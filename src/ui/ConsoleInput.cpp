#include "ConsoleInput.h"

#include "ConsoleTheme.h"
#include "ConsoleView.h"

#include <cstdlib>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

namespace
{
	template <typename T>
	std::string toText(const T& value)
	{
		std::ostringstream output;
		output << value;
		return output.str();
	}
}

char ConsoleInput::menuChoice(int maxChoice)
{
	return static_cast<char>('0' + choice("请选择", maxChoice));
}

int ConsoleInput::choice(const std::string& prompt, int maxChoice)
{
	while (true)
	{
		std::cout << prompt << "(返回请输入0):";
		int value = 0;
		if (std::cin >> value && value >= 0 && value <= maxChoice)
			return value;
		std::cin.clear();
		std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
		ConsoleView::error(ConsoleTheme::invalidMenuChoice());
	}
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
		ConsoleView::error(ConsoleTheme::invalidLineNumber());
	}
}

float ConsoleInput::score(float min, float max, const std::string& label)
{
	while (true)
	{
		std::cout << label << "(" << min << "-" << max << "):";
		float value = 0.0f;
		if (std::cin >> value && value >= min && value <= max)
			return value;
		std::cin.clear();
		std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
		ConsoleView::error("请输入" + toText(min) + "-" + toText(max) + "之间的" + label + "!");
	}
}

std::vector<float> ConsoleInput::moralScores(const std::vector<std::string>& items)
{
	std::vector<float> scores;
	for (size_t index = 0; index < items.size(); ++index)
	{
		while (true)
		{
			std::cout << "第" << index + 1 << "项(0-10)" << std::endl;
			std::cout << items[index] << ":";
			float value = 0.0f;
			if (std::cin >> value && value >= 0.0f && value <= 10.0f)
			{
				scores.push_back(value);
				break;
			}
			std::cin.clear();
			std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
			ConsoleView::error("请输入0-10之间的分数!");
		}
	}
	return scores;
}

bool ConsoleInput::confirm(const std::string& prompt)
{
	while (true)
	{
		std::cout << prompt << "(1确认/0取消):";
		int value = -1;
		if (std::cin >> value && (value == 0 || value == 1))
			return value == 1;
		std::cin.clear();
		std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
		ConsoleView::error("请输入1确认或0取消!");
	}
}

const std::vector<std::string>& ConsoleInput::studentMoralItems()
{
	static const std::vector<std::string> items = {
		"严格遵守校规校纪及学院各项规章制度",
		"深入科研实践、勤奋学习",
		"合理安排课余时间，按时自习，不沉迷游戏，不过滥用电脑",
		"生活简朴节约，为人诚实守信",
		"注意保持个人卫生，爱护环境，维护公物",
		"积极配合并参加班级与学院组织的各项活动，有责任感",
		"积极参加体育锻炼",
		"培养自身素质，积极参加各项社会实践活动和志愿服务活动",
		"尊敬师长，关心集体，文明礼貌，团结同学，乐于助人"
	};
	return items;
}

const std::vector<std::string>& ConsoleInput::teacherMoralItems()
{
	static const std::vector<std::string> items = {
		"学生日常学习情况，上课出勤率，学习认真程度",
		"学生平时生活情况，自律、诚信情况",
		"学生参与班会及班级集体活动情况"
	};
	return items;
}

std::string ConsoleInput::text(const std::string& prompt)
{
	return textRequired(prompt);
}

std::string ConsoleInput::textRequired(const std::string& prompt)
{
	while (true)
	{
		std::cout << prompt;
		std::string value;
		if (std::cin >> value && !value.empty())
			return value;
		std::cin.clear();
		std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
		ConsoleView::error("输入不能为空!");
	}
}

std::string ConsoleInput::optionalText(const std::string& prompt)
{
	std::cout << prompt;
	std::string value;
	std::cin >> value;
	if (value == "0")
		return "";
	return value;
}

void ConsoleInput::pause()
{
	std::system("pause");
}
