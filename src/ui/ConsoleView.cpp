#include "ConsoleView.h"

#include "ActivityRecord.h"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace
{
	int displayWidth(const std::string& value)
	{
		int width = 0;
		for (size_t index = 0; index < value.size();)
		{
			const unsigned char ch = static_cast<unsigned char>(value[index]);
			if (ch < 0x80)
			{
				++width;
				++index;
			}
			else
			{
				width += 2;
				if ((ch & 0xE0) == 0xC0)
					index += 2;
				else if ((ch & 0xF0) == 0xE0)
					index += 3;
				else if ((ch & 0xF8) == 0xF0)
					index += 4;
				else
					++index;
			}
		}
		return width;
	}

	template <typename T>
	std::string toText(const T& value)
	{
		std::ostringstream output;
		output << value;
		return output.str();
	}

	void cell(const std::string& value, int width)
	{
		std::cout << value;
		const int spaces = width - displayWidth(value);
		std::cout << std::string(spaces > 0 ? spaces : 1, ' ');
	}
}

void ConsoleView::clear()
{
	std::system("cls");
}

void ConsoleView::header()
{
	clear();
	std::cout << "*****************************************************" << std::endl
		<< "**                                                 **" << std::endl
		<< "**         欢迎进入班级综合素质测评管理系统        **" << std::endl
		<< "**                                                 **" << std::endl
		<< "*****************************************************" << std::endl;
}

void ConsoleView::message(const std::string& value)
{
	std::cout << value << std::endl;
}

void ConsoleView::error(const std::string& value)
{
	std::cout << value << std::endl;
}

void ConsoleView::menu(const std::vector<std::string>& items)
{
	header();
	for (size_t index = 0; index < items.size(); ++index)
		std::cout << '[' << index + 1 << "] " << items[index] << std::endl;
}

void ConsoleView::courses(const std::vector<CourseRecord>& records)
{
	cell("行号", 8);
	cell("学号", 10);
	cell("课程名", 20);
	cell("学分", 8);
	cell("成绩", 8);
	std::cout << std::endl;
	for (size_t index = 0; index < records.size(); ++index)
	{
		cell(toText(index + 1), 8);
		cell(records[index].account, 10);
		cell(records[index].name, 20);
		cell(toText(records[index].credit), 8);
		cell(toText(records[index].grade), 8);
		std::cout << std::endl;
	}
}

void ConsoleView::activities(const std::vector<ActivityRecord>& records)
{
	cell("行数", 8);
	cell("学号", 10);
	cell("活动名", 20);
	cell("类型", 12);
	cell("分数", 8);
	cell("状态", 14);
	std::cout << std::endl;
	for (size_t index = 0; index < records.size(); ++index)
	{
		cell(toText(index + 1), 8);
		cell(records[index].account, 10);
		cell(records[index].name, 20);
		cell(records[index].type, 12);
		cell(toText(records[index].grade), 8);
		cell(toFileValue(records[index].status), 14);
		std::cout << std::endl;
	}
}

void ConsoleView::morals(const std::vector<MoralRecord>& records)
{
	cell("行数", 8);
	cell("得分人", 12);
	cell("打分人", 12);
	cell("总分", 8);
	std::cout << std::endl;
	for (size_t index = 0; index < records.size(); ++index)
	{
		cell(toText(index + 1), 8);
		cell(records[index].receiverAccount, 12);
		cell(records[index].giverAccount, 12);
		cell(toText(sumScores(records[index])), 8);
		std::cout << std::endl;
	}
}

void ConsoleView::scores(const std::vector<StudentScore>& records)
{
	cell("学号", 10);
	cell("学习成绩", 12);
	cell("GPA", 8);
	cell("课外活动成绩", 16);
	cell("思想品德成绩", 16);
	cell("附加分", 10);
	cell("综测成绩", 12);
	cell("排名", 8);
	std::cout << std::endl;
	for (std::vector<StudentScore>::const_iterator iter = records.begin(); iter != records.end(); ++iter)
	{
		cell(iter->account, 10);
		cell(toText(iter->study), 12);
		cell(toText(iter->gpa), 8);
		cell(toText(iter->activity), 16);
		cell(toText(iter->moral), 16);
		cell(toText(iter->addition), 10);
		cell(toText(iter->total), 12);
		cell(toText(iter->rank), 8);
		std::cout << std::endl;
	}
}
