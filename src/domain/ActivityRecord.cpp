#include "ActivityRecord.h"

#include <istream>
#include <ostream>
#include <stdexcept>

std::string toFileValue(ReviewStatus status)
{
	switch (status)
	{
	case ReviewStatus::Pending:
		return "未审核";
	case ReviewStatus::Approved:
		return "审核通过";
	case ReviewStatus::Rejected:
		return "审核未通过";
	}
	throw std::runtime_error("invalid review status");
}

ReviewStatus reviewStatusFromFileValue(const std::string& value)
{
	if (value == "未审核")
		return ReviewStatus::Pending;
	if (value == "审核通过")
		return ReviewStatus::Approved;
	if (value == "审核未通过")
		return ReviewStatus::Rejected;
	throw std::runtime_error("invalid review status");
}

std::istream& operator>>(std::istream& in, ActivityRecord& record)
{
	std::string status;
	if (in >> record.account >> record.name >> record.type >> record.grade >> status)
		record.status = reviewStatusFromFileValue(status);
	return in;
}

std::ostream& operator<<(std::ostream& out, const ActivityRecord& record)
{
	out << record.account << '\t' << record.name << '\t' << record.type << '\t'
		<< record.grade << '\t' << toFileValue(record.status) << '\n';
	return out;
}
