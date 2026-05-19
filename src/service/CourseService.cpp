#include "CourseService.h"

#include <stdexcept>

namespace
{
	void requireTotalNotGenerated(const AssessmentRepository& repository)
	{
		if (repository.status().totalGenerated)
			throw std::runtime_error("综测成绩已生成，请先撤销后再修改");
	}
}

CourseService::CourseService(AssessmentRepository& repository)
	: repository_(repository)
{
}

std::vector<CourseRecord> CourseService::listAll() const
{
	std::vector<CourseRecord> records;
	for (std::multimap<std::string, CourseRecord>::const_iterator iter = repository_.courses().begin(); iter != repository_.courses().end(); ++iter)
		records.push_back(iter->second);
	return records;
}

void CourseService::createCourse(const std::string& account, const std::string& name, float credit, float grade)
{
	requireTotalNotGenerated(repository_);
	CourseRecord record;
	record.account = account;
	record.name = name;
	record.credit = credit;
	record.grade = grade;
	repository_.courses().insert(std::pair<std::string, CourseRecord>(account, record));
	repository_.saveCourses();
}

void CourseService::updateCourse(int row, const CourseRecord& replacement)
{
	requireTotalNotGenerated(repository_);
	findByGlobalRow(row)->second = replacement;
	repository_.saveCourses();
}

void CourseService::deleteCourse(int row)
{
	requireTotalNotGenerated(repository_);
	repository_.courses().erase(findByGlobalRow(row));
	repository_.saveCourses();
}

std::multimap<std::string, CourseRecord>::iterator CourseService::findByGlobalRow(int row)
{
	if (row <= 0)
		throw std::runtime_error("行数错误");
	for (std::multimap<std::string, CourseRecord>::iterator iter = repository_.courses().begin(); iter != repository_.courses().end(); ++iter)
	{
		--row;
		if (row == 0)
			return iter;
	}
	throw std::runtime_error("行数错误");
}
