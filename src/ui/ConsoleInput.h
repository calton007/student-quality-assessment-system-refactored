#pragma once

#include <string>

class ConsoleInput
{
public:
	static char menuChoice();
	static int lineNumber(int maxLine);
	static float score(float min, float max);
	static std::string text(const std::string& prompt);
	static void pause();
};
