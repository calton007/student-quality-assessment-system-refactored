#pragma once

#include <iosfwd>
#include <string>

struct StudentScore
{
	std::string account;
	float study = 0.0f;
	float gpa = 0.0f;
	float activity = 0.0f;
	float moral = 0.0f;
	float addition = 0.0f;
	float total = 0.0f;
	int rank = 0;
};

std::istream& operator>>(std::istream& in, StudentScore& score);
std::ostream& operator<<(std::ostream& out, const StudentScore& score);
