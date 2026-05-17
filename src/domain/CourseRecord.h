#pragma once

#include <iosfwd>
#include <string>

struct CourseRecord
{
	std::string account;
	std::string name;
	float credit = 0.0f;
	float grade = 0.0f;
};

std::istream& operator>>(std::istream& in, CourseRecord& record);
std::ostream& operator<<(std::ostream& out, const CourseRecord& record);
