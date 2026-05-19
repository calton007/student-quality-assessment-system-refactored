#pragma once

#include "UserRecord.h"

#include <string>

class ConsoleScreen
{
public:
	static void clear();
	static void header();
	static void header(const std::string& location, const UserRecord* user);
};
