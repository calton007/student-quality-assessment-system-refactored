#pragma once

#include <string>
#include <vector>

class ConsoleInput
{
public:
	static char menuChoice();
	static int lineNumber(int maxLine);
	static float score(float min, float max, const std::string& label = "分数");
	static std::vector<float> moralScores(const std::vector<std::string>& items);
	static const std::vector<std::string>& studentMoralItems();
	static const std::vector<std::string>& teacherMoralItems();
	static std::string text(const std::string& prompt);
	static void pause();
};
