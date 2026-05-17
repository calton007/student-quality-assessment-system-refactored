#pragma once

#include "AssessmentRepository.h"

#include <string>
#include <vector>

class CourseService
{
public:
	explicit CourseService(AssessmentRepository& repository);

	std::vector<CourseRecord> listAll() const;
	void createCourse(const std::string& account, const std::string& name, float credit, float grade);
	void updateCourse(int row, const CourseRecord& replacement);
	void deleteCourse(int row);

private:
	std::multimap<std::string, CourseRecord>::iterator findByGlobalRow(int row);

	AssessmentRepository& repository_;
};
