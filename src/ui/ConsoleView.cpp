#include "ConsoleView.h"

#include "ActivityRecord.h"

#include <cstdlib>
#include <iomanip>
#include <iostream>

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
	std::cout << std::setw(8) << "行号" << std::setw(10) << "学号" << std::setw(20) << "课程名"
		<< std::setw(8) << "学分" << std::setw(8) << "成绩" << std::endl;
	for (size_t index = 0; index < records.size(); ++index)
	{
		std::cout << std::setw(8) << index + 1 << std::setw(10) << records[index].account
			<< std::setw(20) << records[index].name << std::setw(8) << records[index].credit
			<< std::setw(8) << records[index].grade << std::endl;
	}
}

void ConsoleView::activities(const std::vector<ActivityRecord>& records)
{
	std::cout << std::setw(8) << "行数" << std::setw(10) << "学号" << std::setw(20) << "活动名"
		<< std::setw(12) << "类型" << std::setw(8) << "分数" << std::setw(14) << "状态" << std::endl;
	for (size_t index = 0; index < records.size(); ++index)
	{
		std::cout << std::setw(8) << index + 1 << std::setw(10) << records[index].account
			<< std::setw(20) << records[index].name << std::setw(12) << records[index].type
			<< std::setw(8) << records[index].grade << std::setw(14) << toFileValue(records[index].status) << std::endl;
	}
}

void ConsoleView::morals(const std::vector<MoralRecord>& records)
{
	std::cout << std::setw(8) << "行数" << std::setw(12) << "得分人" << std::setw(12) << "打分人" << std::setw(8) << "总分" << std::endl;
	for (size_t index = 0; index < records.size(); ++index)
		std::cout << std::setw(8) << index + 1 << std::setw(12) << records[index].receiverAccount
			<< std::setw(12) << records[index].giverAccount << std::setw(8) << sumScores(records[index]) << std::endl;
}

void ConsoleView::scores(const std::vector<StudentScore>& records)
{
	std::cout << std::setw(8) << "学号" << std::setw(12) << "学习成绩" << std::setw(7) << "GPA"
		<< std::setw(14) << "课外活动成绩" << std::setw(14) << "思想品德成绩" << std::setw(10)
		<< "附加分" << std::setw(12) << "综测成绩" << std::setw(8) << "排名" << std::endl;
	for (std::vector<StudentScore>::const_iterator iter = records.begin(); iter != records.end(); ++iter)
	{
		std::cout << std::setw(8) << iter->account << std::setw(12) << iter->study << std::setw(7) << iter->gpa
			<< std::setw(14) << iter->activity << std::setw(14) << iter->moral << std::setw(10)
			<< iter->addition << std::setw(12) << iter->total << std::setw(8) << iter->rank << std::endl;
	}
}
