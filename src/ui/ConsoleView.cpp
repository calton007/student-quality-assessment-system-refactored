#include "ConsoleView.h"

#include "ActivityRecord.h"
#include "ConsoleMenu.h"
#include "ConsoleScreen.h"
#include "ConsoleTable.h"

#include <iostream>
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

void ConsoleView::clear()
{
	ConsoleScreen::clear();
}

void ConsoleView::header()
{
	ConsoleScreen::header();
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
	ConsoleMenu::render(items);
}

void ConsoleView::menu(const std::string& location, const UserRecord& user, const std::vector<std::string>& items)
{
	ConsoleMenu::render(location, user, items);
}

void ConsoleView::operation(const std::string& studentName, const std::string& studentAccount, const std::string& item, const std::string& action)
{
	std::cout << "正在操作: " << studentName << "(" << studentAccount << ") / " << item << " / " << action << std::endl;
}

void ConsoleView::courses(const std::vector<CourseRecord>& records)
{
	std::vector<std::vector<std::string>> rows;
	for (size_t index = 0; index < records.size(); ++index)
	{
		std::vector<std::string> row;
		row.push_back(toText(index + 1));
		row.push_back(records[index].account);
		row.push_back(records[index].name);
		row.push_back(toText(records[index].credit));
		row.push_back(toText(records[index].grade));
		rows.push_back(row);
	}
	ConsoleTable::render({ ConsoleTable::Column("行号", 8), ConsoleTable::Column("学号", 10), ConsoleTable::Column("课程名", 20), ConsoleTable::Column("学分", 8), ConsoleTable::Column("成绩", 8) }, rows);
}

void ConsoleView::activities(const std::vector<ActivityRecord>& records)
{
	std::vector<std::vector<std::string>> rows;
	for (size_t index = 0; index < records.size(); ++index)
	{
		std::vector<std::string> row;
		row.push_back(toText(index + 1));
		row.push_back(records[index].account);
		row.push_back(records[index].name);
		row.push_back(records[index].type);
		row.push_back(toText(records[index].grade));
		row.push_back(toFileValue(records[index].status));
		rows.push_back(row);
	}
	ConsoleTable::render({ ConsoleTable::Column("行号", 8), ConsoleTable::Column("学号", 10), ConsoleTable::Column("活动名", 20), ConsoleTable::Column("类型", 12), ConsoleTable::Column("分数", 8), ConsoleTable::Column("状态", 14) }, rows);
}

void ConsoleView::morals(const std::vector<MoralRecord>& records)
{
	std::vector<std::vector<std::string>> rows;
	for (size_t index = 0; index < records.size(); ++index)
	{
		std::vector<std::string> row;
		row.push_back(toText(index + 1));
		row.push_back(records[index].receiverAccount);
		row.push_back(records[index].giverAccount);
		row.push_back(toText(sumScores(records[index])));
		rows.push_back(row);
	}
	ConsoleTable::render({ ConsoleTable::Column("行号", 8), ConsoleTable::Column("得分人", 12), ConsoleTable::Column("打分人", 12), ConsoleTable::Column("总分", 8) }, rows);
}

void ConsoleView::scores(const std::vector<StudentScore>& records)
{
	std::vector<std::vector<std::string>> rows;
	for (std::vector<StudentScore>::const_iterator iter = records.begin(); iter != records.end(); ++iter)
	{
		std::vector<std::string> row;
		row.push_back(iter->account);
		row.push_back(toText(iter->study));
		row.push_back(toText(iter->gpa));
		row.push_back(toText(iter->activity));
		row.push_back(toText(iter->moral));
		row.push_back(toText(iter->addition));
		row.push_back(toText(iter->total));
		row.push_back(toText(iter->rank));
		rows.push_back(row);
	}
	ConsoleTable::render({ ConsoleTable::Column("学号", 10), ConsoleTable::Column("学习成绩", 12), ConsoleTable::Column("GPA", 8), ConsoleTable::Column("课外活动成绩", 16), ConsoleTable::Column("思想品德成绩", 16), ConsoleTable::Column("附加分", 10), ConsoleTable::Column("综测成绩", 12), ConsoleTable::Column("排名", 8) }, rows);
}
