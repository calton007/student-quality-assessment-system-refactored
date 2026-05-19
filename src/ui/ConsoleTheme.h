#pragma once

#include <string>

class ConsoleTheme
{
public:
	static const std::string& title();
	static const std::string& separator();
	static const std::string& emptyData();
	static const std::string& invalidMenuChoice();
	static const std::string& invalidLineNumber();
};
