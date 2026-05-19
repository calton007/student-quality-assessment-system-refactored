#pragma once

#include "UserRecord.h"

#include <string>
#include <vector>

class ConsoleMenu
{
public:
	static void render(const std::vector<std::string>& items);
	static void render(const std::string& location, const UserRecord& user, const std::vector<std::string>& items);
};
