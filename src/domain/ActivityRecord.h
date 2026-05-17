#pragma once

#include <iosfwd>
#include <string>

enum class ReviewStatus
{
	Pending,
	Approved,
	Rejected
};

struct ActivityRecord
{
	std::string account;
	std::string name;
	std::string type;
	float grade = 0.0f;
	ReviewStatus status = ReviewStatus::Pending;
};

std::string toFileValue(ReviewStatus status);
ReviewStatus reviewStatusFromFileValue(const std::string& value);

std::istream& operator>>(std::istream& in, ActivityRecord& record);
std::ostream& operator<<(std::ostream& out, const ActivityRecord& record);
