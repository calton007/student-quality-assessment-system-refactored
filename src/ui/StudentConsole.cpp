#include "StudentConsole.h"

#include "ActivityService.h"
#include "AuthService.h"
#include "ConsoleInput.h"
#include "ConsoleView.h"
#include "MoralService.h"
#include "QueryService.h"

#include <iostream>

StudentConsole::StudentConsole(AssessmentRepository& repository, const UserRecord& user)
	: repository_(repository), user_(user)
{
}

void StudentConsole::run()
{
	while (true)
	{
		ConsoleView::menu("学生首页", user_, { "思想品德项目", "课外活动项目", "查询", "修改密码", "返回登陆界面", "退出系统" });
		switch (ConsoleInput::menuChoice(6))
		{
		case '1': moralMenu(); break;
		case '2': activityMenu(); break;
		case '3': searchMenu(); break;
		case '4': changePassword(); break;
		case '0':
		case '5': return;
		case '6': std::exit(0);
		default: ConsoleView::message("您的输入有误,请重新输入!"); ConsoleInput::pause(); break;
		}
	}
}

void StudentConsole::moralMenu()
{
	ConsoleView::menu("学生首页 / 思想品德项目", user_, { "思想品德打分", "修改思想品德打分", "返回" });
	switch (ConsoleInput::menuChoice(3))
	{
	case '1': gradeMoral(); break;
	case '2': modifyMoral(); break;
	case '0':
	default: break;
	}
}

void StudentConsole::gradeMoral()
{
	try
	{
		if (repository_.status().studentMoralFinishedAccounts.find(user_.account) != repository_.status().studentMoralFinishedAccounts.end())
			throw std::runtime_error("已打分!");
		std::vector<MoralRecord> records;
		for (std::map<std::string, UserRecord>::const_iterator iter = repository_.users().begin(); iter != repository_.users().end(); ++iter)
		{
			if (iter->second.role != UserRole::Student)
				continue;
			ConsoleView::menu("学生首页 / 思想品德项目 / 思想品德打分", user_, std::vector<std::string>());
			ConsoleView::operation(iter->second.name, iter->first, "思想品德", "打分");
			MoralRecord record;
			record.receiverAccount = iter->first;
			record.giverAccount = user_.account;
			record.scores = ConsoleInput::moralScores(ConsoleInput::studentMoralItems());
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
		ConsoleView::menu("学生首页 / 思想品德项目 / 修改思想品德打分", user_, std::vector<std::string>());
		ConsoleView::morals(records);
		if (records.empty())
			throw std::runtime_error("没有可修改的思想品德评分记录!");
		int row = ConsoleInput::lineNumber(static_cast<int>(records.size()));
		if (row == 0)
			return;
		MoralRecord record = records[row - 1];
		record.scores = ConsoleInput::moralScores(ConsoleInput::studentMoralItems());
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
	ConsoleView::menu("学生首页 / 课外活动项目", user_, { "录入课外活动", "修改课外活动", "删除课外活动", "查询课外活动", "返回" });
	switch (ConsoleInput::menuChoice(5))
	{
	case '1': newActivity(); break;
	case '2': modifyActivity(); break;
	case '3': deleteActivity(); break;
	case '4': displayActivity(); break;
	case '0':
	default: break;
	}
}

void StudentConsole::newActivity()
{
	try
	{
		ConsoleView::menu("学生首页 / 课外活动项目 / 录入课外活动", user_, std::vector<std::string>());
		ConsoleView::operation(user_.name, user_.account, "课外活动", "录入");
		std::string name = ConsoleInput::optionalText("活动名称(返回请输入0):");
		if (name.empty())
			return;
		ActivityService(repository_).createActivity(user_.account, name, ConsoleInput::score(0.5f, 20.0f, "活动分"));
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
		ConsoleView::menu("学生首页 / 课外活动项目 / 修改课外活动", user_, std::vector<std::string>());
		ConsoleView::activities(records);
		int row = ConsoleInput::lineNumber(static_cast<int>(records.size()));
		if (row == 0)
			return;
		ConsoleView::operation(user_.name, user_.account, "课外活动", "修改");
		std::string name = ConsoleInput::optionalText("活动名称(返回请输入0):");
		if (name.empty())
			return;
		service.updateStudentActivity(user_.account, row, name, ConsoleInput::score(0.5f, 20.0f, "活动分"));
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
		ConsoleView::menu("学生首页 / 课外活动项目 / 删除课外活动", user_, std::vector<std::string>());
		ConsoleView::activities(records);
		int row = ConsoleInput::lineNumber(static_cast<int>(records.size()));
		if (row == 0)
			return;
		ConsoleView::operation(user_.name, user_.account, "课外活动", "删除");
		service.deleteStudentActivity(user_.account, row);
		ConsoleView::message("删除成功!");
	}
	catch (const std::exception& ex) { ConsoleView::error(ex.what()); }
	ConsoleInput::pause();
}

void StudentConsole::displayActivity()
{
	ConsoleView::menu("学生首页 / 课外活动项目 / 查询课外活动", user_, std::vector<std::string>());
	ConsoleView::activities(ActivityService(repository_).listForStudent(user_.account));
	ConsoleInput::pause();
}

void StudentConsole::searchMenu()
{
	try
	{
		QueryService query(repository_);
		if (!query.totalGenerated())
			throw std::runtime_error("尚未生成综测成绩,无法查询!");
		const StudentScore& score = query.scoreFor(user_.account);
		ConsoleView::menu("学生首页 / 查询", user_, std::vector<std::string>());
		ConsoleView::scores(std::vector<StudentScore>(1, score));
	}
	catch (const std::exception& ex) { ConsoleView::error(ex.what()); }
	ConsoleInput::pause();
}

void StudentConsole::changePassword()
{
	try
	{
		ConsoleView::menu("学生首页 / 修改密码", user_, std::vector<std::string>());
		AuthService(repository_).changePassword(user_.account, ConsoleInput::textRequired("请输入原密码:"), ConsoleInput::textRequired("请输入新密码:"));
		ConsoleView::message("修改成功!");
	}
	catch (const std::exception& ex) { ConsoleView::error(ex.what()); }
	ConsoleInput::pause();
}
