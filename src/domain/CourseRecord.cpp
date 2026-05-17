#include "CourseRecord.h"

#include <istream>
#include <ostream>

std::istream& operator>>(std::istream& in, CourseRecord& record)
{
	in >> record.account >> record.name >> record.credit >> record.grade;
	return in;
}

std::ostream& operator<<(std::ostream& out, const CourseRecord& record)
{
	out << record.account << '\t' << record.name << '\t' << record.credit << '\t' << record.grade << '\n';
	return out;
}
