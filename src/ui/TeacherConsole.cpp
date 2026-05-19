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
		ConsoleView::menu({ "思想品德打分", "修改思想品德打分", "修改密码", "返回登陆界面", "退出系统" });
		switch (ConsoleInput::menuChoice(5))
		{
		case '1': gradeMoral(); break;
		case '2': modifyMoral(); break;
		case '3': changePassword(); break;
		case '0':
		case '4': return;
		case '5': std::exit(0);
		default: ConsoleView::message("您的输入有误,请重新输入!"); ConsoleInput::pause(); break;
		}
	}
}

void TeacherConsole::gradeMoral()
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
		ConsoleView::header();
		ConsoleView::morals(records);
		if (records.empty())
			throw std::runtime_error("没有可修改的思想品德评分记录!");
		int row = ConsoleInput::lineNumber(static_cast<int>(records.size()));
		if (row == 0)
			return;
		MoralRecord record = records[row - 1];
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
		AuthService(repository_).changePassword(user_.account, ConsoleInput::text("请输入原密码:"), ConsoleInput::text("请输入新密码:"));
		ConsoleView::message("修改成功!");
	}
	catch (const std::exception& ex) { ConsoleView::error(ex.what()); }
	ConsoleInput::pause();
}
