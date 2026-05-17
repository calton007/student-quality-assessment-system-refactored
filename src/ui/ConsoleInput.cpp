#include "ConsoleInput.h"

#include <cstdlib>
#include <iostream>
#include <limits>

char ConsoleInput::menuChoice()
{
	char choice = '\0';
	std::cout << "请选择:";
	std::cin >> choice;
	return choice;
}

int ConsoleInput::lineNumber(int maxLine)
{
	while (true)
	{
		std::cout << "请输入行数(返回请输入0):";
		int value = 0;
		if (std::cin >> value && value >= 0 && value <= maxLine)
			return value;
		std::cin.clear();
		std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
		std::cout << "请输入正确的行数!" << std::endl;
	}
}

float ConsoleInput::score(float min, float max)
{
	while (true)
	{
		std::cout << "分数(" << min << "-" << max << "):";
		float value = 0.0f;
		if (std::cin >> value && value >= min && value <= max)
			return value;
		std::cin.clear();
		std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
		std::cout << "请输入" << min << "-" << max << "之间的分数!" << std::endl;
	}
}

std::string ConsoleInput::text(const std::string& prompt)
{
	std::cout << prompt;
	std::string value;
	std::cin >> value;
	return value;
}

void ConsoleInput::pause()
{
	std::system("pause");
}
