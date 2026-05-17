#include "AdditionService.h"

#include <stdexcept>

AdditionService::AdditionService(AssessmentRepository& repository)
	: repository_(repository)
{
}

std::vector<ActivityRecord> AdditionService::listAll() const
{
	std::vector<ActivityRecord> records;
	for (std::multimap<std::string, ActivityRecord>::const_iterator iter = repository_.additions().begin(); iter != repository_.additions().end(); ++iter)
		records.push_back(iter->second);
	return records;
}

void AdditionService::createPersonalAddition(const std::string& account, const std::string& name, float grade)
{
	ActivityRecord record;
	record.account = account;
	record.name = name;
	record.type = "附加分";
	record.grade = grade;
	record.status = ReviewStatus::Approved;
	repository_.additions().insert(std::pair<std::string, ActivityRecord>(account, record));
	repository_.saveAdditions();
}

void AdditionService::createGroupAddition(const std::string& name, float grade)
{
	for (std::map<std::string, UserRecord>::const_iterator iter = repository_.users().begin(); iter != repository_.users().end(); ++iter)
	{
		if (iter->second.role == UserRole::Student)
			createPersonalAddition(iter->first, name, grade);
	}
}

void AdditionService::updateAddition(int row, const ActivityRecord& replacement)
{
	ActivityRecord record = replacement;
	record.type = "附加分";
	record.status = ReviewStatus::Approved;
	findByGlobalRow(row)->second = record;
	repository_.saveAdditions();
}

void AdditionService::deleteAddition(int row)
{
	repository_.additions().erase(findByGlobalRow(row));
	repository_.saveAdditions();
}

std::multimap<std::string, ActivityRecord>::iterator AdditionService::findByGlobalRow(int row)
{
	if (row <= 0)
		throw std::runtime_error("行数错误");
	for (std::multimap<std::string, ActivityRecord>::iterator iter = repository_.additions().begin(); iter != repository_.additions().end(); ++iter)
	{
		--row;
		if (row == 0)
			return iter;
	}
	throw std::runtime_error("行数错误");
}
