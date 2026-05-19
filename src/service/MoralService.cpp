#include "MoralService.h"

#include <stdexcept>

namespace
{
	void requireTotalNotGenerated(const AssessmentRepository& repository)
	{
		if (repository.status().totalGenerated)
			throw std::runtime_error("综测成绩已生成，请先撤销后再修改");
	}
}

MoralService::MoralService(AssessmentRepository& repository)
	: repository_(repository)
{
}

float MoralService::total(const MoralRecord& record)
{
	return sumScores(record);
}

std::vector<MoralRecord> MoralService::studentMoralsGivenBy(const std::string& giverAccount) const
{
	std::vector<MoralRecord> records;
	std::pair<std::multimap<std::string, MoralRecord>::const_iterator, std::multimap<std::string, MoralRecord>::const_iterator> range =
		repository_.studentMoralsByGiver().equal_range(giverAccount);
	for (; range.first != range.second; ++range.first)
		records.push_back(range.first->second);
	return records;
}

std::vector<MoralRecord> MoralService::teacherMorals() const
{
	std::vector<MoralRecord> records;
	for (std::map<std::string, MoralRecord>::const_iterator iter = repository_.teacherMorals().begin(); iter != repository_.teacherMorals().end(); ++iter)
		records.push_back(iter->second);
	return records;
}

void MoralService::submitStudentMoral(const std::string& giverAccount, const std::vector<MoralRecord>& records)
{
	requireTotalNotGenerated(repository_);
	if (repository_.studentMoralsByGiver().find(giverAccount) != repository_.studentMoralsByGiver().end())
		throw std::runtime_error("已提交过学生互评，不能重复提交");
	for (std::vector<MoralRecord>::const_iterator iter = records.begin(); iter != records.end(); ++iter)
	{
		MoralRecord record = *iter;
		record.giverAccount = giverAccount;
		repository_.studentMoralsByReceiver().insert(std::pair<std::string, MoralRecord>(record.receiverAccount, record));
		repository_.studentMoralsByGiver().insert(std::pair<std::string, MoralRecord>(record.giverAccount, record));
	}
	repository_.saveStudentMorals();
	repository_.status().studentMoralFinishedAccounts.insert(giverAccount);
	repository_.saveStatus();
}

void MoralService::submitTeacherMoral(const std::string& teacherAccount, const std::vector<MoralRecord>& records)
{
	requireTotalNotGenerated(repository_);
	for (std::vector<MoralRecord>::const_iterator iter = records.begin(); iter != records.end(); ++iter)
	{
		MoralRecord record = *iter;
		record.giverAccount = teacherAccount;
		repository_.teacherMorals()[record.receiverAccount] = record;
	}
	repository_.saveTeacherMorals();
	repository_.status().teacherMoralFinishedAccounts.insert(teacherAccount);
	repository_.saveStatus();
}

void MoralService::updateStudentMoral(const std::string& giverAccount, int row, const MoralRecord& record)
{
	requireTotalNotGenerated(repository_);
	std::multimap<std::string, MoralRecord>::iterator giverIter = findStudentMoralByGiverRow(giverAccount, row);
	const std::string oldReceiver = giverIter->second.receiverAccount;
	giverIter->second = record;
	giverIter->second.giverAccount = giverAccount;

	std::pair<std::multimap<std::string, MoralRecord>::iterator, std::multimap<std::string, MoralRecord>::iterator> range =
		repository_.studentMoralsByReceiver().equal_range(oldReceiver);
	for (; range.first != range.second; ++range.first)
	{
		if (range.first->second.giverAccount == giverAccount)
		{
			range.first->second = giverIter->second;
			break;
		}
	}
	repository_.saveStudentMorals();
}

void MoralService::updateTeacherMoral(int row, const MoralRecord& record)
{
	requireTotalNotGenerated(repository_);
	std::map<std::string, MoralRecord>::iterator iter = findTeacherMoralByRow(row);
	repository_.teacherMorals().erase(iter);
	repository_.teacherMorals()[record.receiverAccount] = record;
	repository_.saveTeacherMorals();
}

std::multimap<std::string, MoralRecord>::iterator MoralService::findStudentMoralByGiverRow(const std::string& giverAccount, int row)
{
	if (row <= 0)
		throw std::runtime_error("行数错误");
	std::pair<std::multimap<std::string, MoralRecord>::iterator, std::multimap<std::string, MoralRecord>::iterator> range =
		repository_.studentMoralsByGiver().equal_range(giverAccount);
	for (; range.first != range.second; ++range.first)
	{
		--row;
		if (row == 0)
			return range.first;
	}
	throw std::runtime_error("行数错误");
}

std::map<std::string, MoralRecord>::iterator MoralService::findTeacherMoralByRow(int row)
{
	if (row <= 0)
		throw std::runtime_error("行数错误");
	for (std::map<std::string, MoralRecord>::iterator iter = repository_.teacherMorals().begin(); iter != repository_.teacherMorals().end(); ++iter)
	{
		--row;
		if (row == 0)
			return iter;
	}
	throw std::runtime_error("行数错误");
}
