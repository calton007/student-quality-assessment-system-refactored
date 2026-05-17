#include "ActivityService.h"

#include <stdexcept>

ActivityService::ActivityService(AssessmentRepository& repository)
	: repository_(repository)
{
}

std::vector<ActivityRecord> ActivityService::listForStudent(const std::string& account) const
{
	std::vector<ActivityRecord> records;
	std::pair<std::multimap<std::string, ActivityRecord>::const_iterator, std::multimap<std::string, ActivityRecord>::const_iterator> range =
		repository_.activities().equal_range(account);
	for (; range.first != range.second; ++range.first)
		records.push_back(range.first->second);
	return records;
}

std::vector<ActivityRecord> ActivityService::listAll() const
{
	std::vector<ActivityRecord> records;
	for (std::multimap<std::string, ActivityRecord>::const_iterator iter = repository_.activities().begin(); iter != repository_.activities().end(); ++iter)
		records.push_back(iter->second);
	return records;
}

void ActivityService::createActivity(const std::string& account, const std::string& name, float grade)
{
	ActivityRecord record;
	record.account = account;
	record.name = name;
	record.type = "课外活动";
	record.grade = grade;
	record.status = ReviewStatus::Pending;
	repository_.activities().insert(std::pair<std::string, ActivityRecord>(account, record));
	repository_.saveActivities();
}

void ActivityService::updateStudentActivity(const std::string& account, int row, const std::string& name, float grade)
{
	std::multimap<std::string, ActivityRecord>::iterator iter = findStudentActivity(account, row);
	if (iter->second.status == ReviewStatus::Approved)
		throw std::runtime_error("审核已通过项目不能修改");
	iter->second.name = name;
	iter->second.grade = grade;
	iter->second.status = ReviewStatus::Pending;
	repository_.saveActivities();
}

void ActivityService::deleteStudentActivity(const std::string& account, int row)
{
	std::multimap<std::string, ActivityRecord>::iterator iter = findStudentActivity(account, row);
	if (iter->second.status == ReviewStatus::Approved)
		throw std::runtime_error("审核已通过项目不能删除");
	repository_.activities().erase(iter);
	repository_.saveActivities();
}

void ActivityService::approveActivity(int row)
{
	std::multimap<std::string, ActivityRecord>::iterator iter = findActivityByGlobalRow(row);
	iter->second.status = ReviewStatus::Approved;
	repository_.saveActivities();
}

void ActivityService::rejectActivity(int row)
{
	std::multimap<std::string, ActivityRecord>::iterator iter = findActivityByGlobalRow(row);
	iter->second.status = ReviewStatus::Rejected;
	iter->second.grade = 0.0f;
	repository_.saveActivities();
}

std::multimap<std::string, ActivityRecord>::iterator ActivityService::findStudentActivity(const std::string& account, int row)
{
	if (row <= 0)
		throw std::runtime_error("行数错误");
	std::pair<std::multimap<std::string, ActivityRecord>::iterator, std::multimap<std::string, ActivityRecord>::iterator> range =
		repository_.activities().equal_range(account);
	for (; range.first != range.second; ++range.first)
	{
		--row;
		if (row == 0)
			return range.first;
	}
	throw std::runtime_error("行数错误");
}

std::multimap<std::string, ActivityRecord>::iterator ActivityService::findActivityByGlobalRow(int row)
{
	if (row <= 0)
		throw std::runtime_error("行数错误");
	for (std::multimap<std::string, ActivityRecord>::iterator iter = repository_.activities().begin(); iter != repository_.activities().end(); ++iter)
	{
		--row;
		if (row == 0)
			return iter;
	}
	throw std::runtime_error("行数错误");
}
