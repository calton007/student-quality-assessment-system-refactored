#include "UserRecord.h"

#include <istream>
#include <ostream>
#include <stdexcept>

char toFileValue(UserRole role)
{
	switch (role)
	{
	case UserRole::Student:
		return '0';
	case UserRole::Teacher:
		return '1';
	case UserRole::Group:
		return '2';
	}
	throw std::runtime_error("invalid user role");
}

UserRole userRoleFromFileValue(char value)
{
	if (value == '0')
		return UserRole::Student;
	if (value == '1')
		return UserRole::Teacher;
	if (value == '2')
		return UserRole::Group;
	throw std::runtime_error("invalid user role");
}

char flagToFileValue(bool value)
{
	return value ? '1' : '0';
}

bool flagFromFileValue(char value)
{
	if (value == '0')
		return false;
	if (value == '1')
		return true;
	throw std::runtime_error("invalid flag");
}

std::istream& operator>>(std::istream& in, UserRecord& record)
{
	char role = '0';
	char flag = '0';
	if (in >> record.account >> record.name >> record.password >> role >> flag)
	{
		record.role = userRoleFromFileValue(role);
		record.finishedMoralOrGeneratedTotal = flagFromFileValue(flag);
	}
	return in;
}

std::ostream& operator<<(std::ostream& out, const UserRecord& record)
{
	out << record.account << '\t' << record.name << '\t' << record.password << '\t'
		<< toFileValue(record.role) << '\t' << flagToFileValue(record.finishedMoralOrGeneratedTotal) << '\n';
	return out;
}
