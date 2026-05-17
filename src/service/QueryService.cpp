#include "QueryService.h"

#include <stdexcept>

QueryService::QueryService(const AssessmentRepository& repository)
	: repository_(repository)
{
}

const StudentScore& QueryService::scoreFor(const std::string& account) const
{
	std::map<std::string, StudentScore>::const_iterator iter = repository_.totals().find(account);
	if (iter == repository_.totals().end())
		throw std::runtime_error("Total.txt 缺少学号 " + account + " 的综测记录");
	return iter->second;
}

std::vector<StudentScore> QueryService::allScores() const
{
	std::vector<StudentScore> scores;
	for (std::map<std::string, StudentScore>::const_iterator iter = repository_.totals().begin(); iter != repository_.totals().end(); ++iter)
		scores.push_back(iter->second);
	return scores;
}

std::string QueryService::userName(const std::string& account) const
{
	std::map<std::string, UserRecord>::const_iterator iter = repository_.users().find(account);
	if (iter == repository_.users().end())
		return "";
	return iter->second.name;
}

bool QueryService::isStudent(const std::string& account) const
{
	std::map<std::string, UserRecord>::const_iterator iter = repository_.users().find(account);
	return iter != repository_.users().end() && iter->second.role == UserRole::Student;
}
