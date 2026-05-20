#include "ScoreDetailService.h"

#include <stdexcept>

ScoreDetailService::ScoreDetailService(const AssessmentRepository& repository)
	: repository_(repository)
{
}

ScoreDetail ScoreDetailService::detailFor(const std::string& account) const
{
	if (!repository_.status().totalGenerated)
		throw std::runtime_error("尚未生成综测成绩,无法查询!");

	std::map<std::string, UserRecord>::const_iterator user = repository_.users().find(account);
	if (user == repository_.users().end() || user->second.role != UserRole::Student)
		throw std::runtime_error("没有学号为" + account + "的学生!");

	std::map<std::string, StudentScore>::const_iterator score = repository_.totals().find(account);
	if (score == repository_.totals().end())
		throw std::runtime_error("缺少该学生的综测成绩");

	ScoreDetail detail;
	detail.user = user->second;
	detail.score = score->second;

	std::pair<std::multimap<std::string, CourseRecord>::const_iterator, std::multimap<std::string, CourseRecord>::const_iterator> courses =
		repository_.courses().equal_range(account);
	for (; courses.first != courses.second; ++courses.first)
		detail.courses.push_back(courses.first->second);

	std::pair<std::multimap<std::string, ActivityRecord>::const_iterator, std::multimap<std::string, ActivityRecord>::const_iterator> activities =
		repository_.activities().equal_range(account);
	for (; activities.first != activities.second; ++activities.first)
		detail.activities.push_back(activities.first->second);

	std::pair<std::multimap<std::string, ActivityRecord>::const_iterator, std::multimap<std::string, ActivityRecord>::const_iterator> additions =
		repository_.additions().equal_range(account);
	for (; additions.first != additions.second; ++additions.first)
		detail.additions.push_back(additions.first->second);

	std::pair<std::multimap<std::string, MoralRecord>::const_iterator, std::multimap<std::string, MoralRecord>::const_iterator> studentMorals =
		repository_.studentMoralsByReceiver().equal_range(account);
	for (; studentMorals.first != studentMorals.second; ++studentMorals.first)
		detail.studentMorals.push_back(studentMorals.first->second);

	std::map<std::string, MoralRecord>::const_iterator teacherMoral = repository_.teacherMorals().find(account);
	if (teacherMoral != repository_.teacherMorals().end())
		detail.teacherMorals.push_back(teacherMoral->second);

	return detail;
}
