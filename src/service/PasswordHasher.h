#pragma once

#include <string>

namespace PasswordHasher
{
	std::string hash(const std::string& password);
	bool verify(const std::string& password, const std::string& storedHash);
	bool isHash(const std::string& value);
}
