#include "StudentConsole.h"

#include "ActivityService.h"
#include "AuthService.h"
#include "ConsoleInput.h"
#include "ConsoleView.h"
#include "MoralService.h"
#include "QueryService.h"

#include <iostream>

namespace
{
	void markTotalNotGenerated(AssessmentRepository& repository)
	{
		for (std::map<std::string, UserRecord>::iterator iter = repository.users().begin(); iter != repository.users().end(); ++iter)
		{
			if (iter->second.role == UserRole::Group)
				iter->second.finishedMoralOrGeneratedTotal = false;
		}
		repository.saveUsers();
	}
}

StudentConsole::StudentConsole(AssessmentRepository& repository, const UserRecord& user)
	: repository_(repository), user_(user)
{
}

void StudentConsole::run()
{
	while (true)
	{
		ConsoleView::menu({ "思想品德项目", "课外活动项目", "查询", "修改密码", "返回登陆界面", "退出系统" });
		switch (ConsoleInput::menuChoice())
		{
		case '1': moralMenu(); break;
		case '2': activityMenu(); break;
		case '3': searchMenu(); break;
		case '4': changePassword(); break;
		case '5': return;
		case '6': std::exit(0);
		default: ConsoleView::message("您的输入有误,请重新输入!"); ConsoleInput::pause(); break;
		}
	}
}

void StudentConsole::moralMenu()
{
	ConsoleView::menu({ "思想品德打分", "修改思想品德打分", "返回" });
	switch (ConsoleInput::menuChoice())
	{
	case '1': gradeMoral(); break;
	case '2': modifyMoral(); break;
	default: break;
	}
}

void StudentConsole::gradeMoral()
{
	try
	{
		if (repository_.users()[user_.account].finishedMoralOrGeneratedTotal)
			throw std::runtime_error("已打分!");
		std::vector<MoralRecord> records;
		for (std::map<std::string, UserRecord>::const_iterator iter = repository_.users().begin(); iter != repository_.users().end(); ++iter)
		{
			if (iter->second.role != UserRole::Student)
				continue;
			ConsoleView::header();
			ConsoleView::message("请给" + iter->second.name + "打分");
			MoralRecord record;
			record.receiverAccount = iter->first;
			record.giverAccount = user_.account;
			for (int index = 0; index < 9; ++index)
				record.scores.push_back(ConsoleInput::score(0.0f, 10.0f));
			records.push_back(record);
		}
		MoralService(repository_).submitStudentMoral(user_.account, records);
		ConsoleView::message("打分完毕!");
	}
	catch (const std::exception& ex)
	{
		ConsoleView::error(ex.what());
	}
	ConsoleInput::pause();
}

void StudentConsole::modifyMoral()
{
	try
	{
		MoralService service(repository_);
		std::vector<MoralRecord> records = service.studentMoralsGivenBy(user_.account);
		ConsoleView::header();
		ConsoleView::morals(records);
		if (records.empty())
			throw std::runtime_error("没有可修改的思想品德评分记录!");
		int row = ConsoleInput::lineNumber(static_cast<int>(records.size()));
		if (row == 0)
			return;
		MoralRecord record = records[row - 1];
		record.scores.clear();
		for (int index = 0; index < 9; ++index)
			record.scores.push_back(ConsoleInput::score(0.0f, 10.0f));
		service.updateStudentMoral(user_.account, row, record);
		ConsoleView::message("修改成功!");
	}
	catch (const std::exception& ex)
	{
		ConsoleView::error(ex.what());
	}
	ConsoleInput::pause();
}

void StudentConsole::activityMenu()
{
	ConsoleView::menu({ "录入课外活动", "修改课外活动", "删除课外活动", "查询课外活动", "返回" });
	switch (ConsoleInput::menuChoice())
	{
	case '1': newActivity(); break;
	case '2': modifyActivity(); break;
	case '3': deleteActivity(); break;
	case '4': displayActivity(); break;
	default: break;
	}
}

void StudentConsole::newActivity()
{
	try
	{
		ActivityService(repository_).createActivity(user_.account, ConsoleInput::text("活动名称:"), ConsoleInput::score(0.5f, 20.0f));
		markTotalNotGenerated(repository_);
		ConsoleView::message("录入成功!");
	}
	catch (const std::exception& ex) { ConsoleView::error(ex.what()); }
	ConsoleInput::pause();
}

void StudentConsole::modifyActivity()
{
	try
	{
		ActivityService service(repository_);
		std::vector<ActivityRecord> records = service.listForStudent(user_.account);
		ConsoleView::header();
		ConsoleView::activities(records);
		int row = ConsoleInput::lineNumber(static_cast<int>(records.size()));
		if (row == 0)
			return;
		service.updateStudentActivity(user_.account, row, ConsoleInput::text("活动名称:"), ConsoleInput::score(0.5f, 20.0f));
		ConsoleView::message("修改成功!");
	}
	catch (const std::exception& ex) { ConsoleView::error(ex.what()); }
	ConsoleInput::pause();
}

void StudentConsole::deleteActivity()
{
	try
	{
		ActivityService service(repository_);
		std::vector<ActivityRecord> records = service.listForStudent(user_.account);
		ConsoleView::header();
		ConsoleView::activities(records);
		int row = ConsoleInput::lineNumber(static_cast<int>(records.size()));
		if (row == 0)
			return;
		service.deleteStudentActivity(user_.account, row);
		ConsoleView::message("删除成功!");
	}
	catch (const std::exception& ex) { ConsoleView::error(ex.what()); }
	ConsoleInput::pause();
}

void StudentConsole::displayActivity()
{
	ConsoleView::header();
	ConsoleView::activities(ActivityService(repository_).listForStudent(user_.account));
	ConsoleInput::pause();
}

void StudentConsole::searchMenu()
{
	try
	{
		QueryService query(repository_);
		const StudentScore& score = query.scoreFor(user_.account);
		ConsoleView::header();
		ConsoleView::scores(std::vector<StudentScore>(1, score));
	}
	catch (const std::exception& ex) { ConsoleView::error(ex.what()); }
	ConsoleInput::pause();
}

void StudentConsole::changePassword()
{
	try
	{
		AuthService(repository_).changePassword(user_.account, ConsoleInput::text("请输入原密码:"), ConsoleInput::text("请输入新密码:"));
		ConsoleView::message("修改成功!");
	}
	catch (const std::exception& ex) { ConsoleView::error(ex.what()); }
	ConsoleInput::pause();
}
