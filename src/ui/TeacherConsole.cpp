#include "TeacherConsole.h"

#include "AuthService.h"
#include "ConsoleInput.h"
#include "ConsoleView.h"
#include "MoralService.h"

#include <cstdlib>
#include <iostream>

TeacherConsole::TeacherConsole(AssessmentRepository& repository, const UserRecord& user)
	: repository_(repository), user_(user)
{
}

void TeacherConsole::run()
{
	while (true)
	{
		const bool totalGenerated = repository_.status().totalGenerated;
		ConsoleView::menu("辅导员首页", user_, homeMenuItems(totalGenerated));
		if (totalGenerated)
		{
			switch (ConsoleInput::menuChoice(3))
			{
			case '1': changePassword(); break;
			case '0':
			case '2': return;
			case '3': std::exit(0);
			default: ConsoleView::error("您的输入有误,请重新输入!"); ConsoleInput::pause(); break;
			}
		}
		else
		{
			switch (ConsoleInput::menuChoice(5))
			{
			case '1': gradeMoral(); break;
			case '2': modifyMoral(); break;
			case '3': changePassword(); break;
			case '0':
			case '4': return;
			case '5': std::exit(0);
			default: ConsoleView::error("您的输入有误,请重新输入!"); ConsoleInput::pause(); break;
			}
		}
	}
}

std::vector<std::string> TeacherConsole::homeMenuItems(bool totalGenerated)
{
	if (totalGenerated)
		return { "修改密码", "返回登陆界面", "退出系统" };
	return { "思想品德打分", "修改思想品德打分", "修改密码", "返回登陆界面", "退出系统" };
}

void TeacherConsole::gradeMoral()
{
	try
	{
		if (repository_.status().teacherMoralFinishedAccounts.find(user_.account) != repository_.status().teacherMoralFinishedAccounts.end())
			throw std::runtime_error("已打分!");
		std::vector<MoralRecord> records;
		for (std::map<std::string, UserRecord>::const_iterator iter = repository_.users().begin(); iter != repository_.users().end(); ++iter)
		{
			if (iter->second.role != UserRole::Student)
				continue;
			ConsoleView::menu("辅导员首页 / 思想品德打分", user_, std::vector<std::string>());
			ConsoleView::operation(iter->second.name, iter->first, "思想品德", "打分");
			MoralRecord record;
			record.receiverAccount = iter->first;
			record.giverAccount = user_.account;
			record.scores = ConsoleInput::moralScores(ConsoleInput::teacherMoralItems());
			records.push_back(record);
		}
		MoralService(repository_).submitTeacherMoral(user_.account, records);
		ConsoleView::message("打分完毕!");
	}
	catch (const std::exception& ex) { ConsoleView::error(ex.what()); }
	ConsoleInput::pause();
}

void TeacherConsole::modifyMoral()
{
	try
	{
		MoralService service(repository_);
		std::vector<MoralRecord> records = service.teacherMorals();
		ConsoleView::menu("辅导员首页 / 修改思想品德打分", user_, std::vector<std::string>());
		ConsoleView::morals(records);
		if (records.empty())
			throw std::runtime_error("没有可修改的思想品德评分记录!");
		int row = ConsoleInput::lineNumber(static_cast<int>(records.size()));
		if (row == 0)
			return;
		MoralRecord record = records[row - 1];
		auto iterator = repository_.users().find(record.receiverAccount);
		if (iterator != repository_.users().end())
			ConsoleView::operation(iterator->second.name, record.receiverAccount, "思想品德", "修改");
		record.scores = ConsoleInput::moralScores(ConsoleInput::teacherMoralItems());
		service.updateTeacherMoral(row, record);
		ConsoleView::message("修改成功!");
	}
	catch (const std::exception& ex) { ConsoleView::error(ex.what()); }
	ConsoleInput::pause();
}

void TeacherConsole::changePassword()
{
	try
	{
		ConsoleView::menu("辅导员首页 / 修改密码", user_, std::vector<std::string>());
		AuthService(repository_).changePassword(user_.account, ConsoleInput::textRequired("请输入原密码:"), ConsoleInput::textRequired("请输入新密码:"));
		ConsoleView::message("修改成功!");
	}
	catch (const std::exception& ex) { ConsoleView::error(ex.what()); }
	ConsoleInput::pause();
}
