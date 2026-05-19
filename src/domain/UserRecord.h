#pragma once

#include <iosfwd>
#include <string>

enum class UserRole
{
	Student,
	Teacher,
	Group
};

struct UserRecord
{
	std::string account;
	std::string name;
	std::string password;
	UserRole role;
};

char toFileValue(UserRole role);
UserRole userRoleFromFileValue(char value);

std::istream& operator>>(std::istream& in, UserRecord& record);
std::ostream& operator<<(std::ostream& out, const UserRecord& record);
