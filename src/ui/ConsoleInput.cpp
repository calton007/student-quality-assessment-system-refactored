#include "ConsoleInput.h"

#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

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
		std::cout << "请输入" << min << "-" << max << "之间的" << label << "!" << std::endl;
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
			std::cout << "请输入0-10之间的分数!" << std::endl;
		}
	}
	return scores;
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
	std::cout << prompt;
	std::string value;
	std::cin >> value;
	return value;
}

void ConsoleInput::pause()
{
	std::system("pause");
}
