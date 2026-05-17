#include "GroupConsole.h"

#include "ActivityService.h"
#include "AdditionService.h"
#include "AuthService.h"
#include "ConsoleInput.h"
#include "ConsoleView.h"
#include "CourseService.h"
#include "QueryService.h"
#include "ScoreService.h"

#include <cstdlib>
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

GroupConsole::GroupConsole(AssessmentRepository& repository, const UserRecord& user)
	: repository_(repository), user_(user)
{
}

void GroupConsole::run()
{
	while (true)
	{
		ConsoleView::menu({ "学习成绩管理", "附加分管理", "审核课外活动加分", "查询项目", "综测成绩生成", "修改密码", "返回登陆界面", "退出系统" });
		switch (ConsoleInput::menuChoice())
		{
		case '1': studyMenu(); break;
		case '2': additionMenu(); break;
		case '3': checkActivity(); break;
		case '4': searchMenu(); break;
		case '5': buildTotal(); break;
		case '6': changePassword(); break;
		case '7': return;
		case '8': std::exit(0);
		default: ConsoleView::message("您的输入有误,请重新输入!"); ConsoleInput::pause(); break;
		}
	}
}

void GroupConsole::studyMenu()
{
	ConsoleView::menu({ "录入学习成绩", "修改学习成绩", "删除学习成绩", "返回" });
	switch (ConsoleInput::menuChoice())
	{
	case '1': newStudy(); break;
	case '2': modifyStudy(); break;
	case '3': deleteStudy(); break;
	default: break;
	}
}

void GroupConsole::newStudy()
{
	try
	{
		CourseService service(repository_);
		std::string mode = ConsoleInput::text("1:批量录入成绩 2:单个录入成绩 其他:返回\n请选择:");
		if (mode == "2")
		{
			std::string account = ConsoleInput::text("录入学生的学号:");
			if (!requireStudent(account))
				return;
			service.createCourse(account, ConsoleInput::text("课程名:"), ConsoleInput::score(0.5f, 10.0f), ConsoleInput::score(0.0f, 100.0f));
		}
		else if (mode == "1")
		{
			std::string name = ConsoleInput::text("课程名:");
			float credit = ConsoleInput::score(0.5f, 10.0f);
			for (std::map<std::string, UserRecord>::const_iterator iter = repository_.users().begin(); iter != repository_.users().end(); ++iter)
			{
				if (iter->second.role == UserRole::Student)
					service.createCourse(iter->first, name, credit, ConsoleInput::score(0.0f, 100.0f));
			}
		}
		markTotalNotGenerated(repository_);
		ConsoleView::message("录入成功!");
	}
	catch (const std::exception& ex) { ConsoleView::error(ex.what()); }
	ConsoleInput::pause();
}

void GroupConsole::modifyStudy()
{
	try
	{
		CourseService service(repository_);
		std::vector<CourseRecord> records = service.listAll();
		ConsoleView::header();
		ConsoleView::courses(records);
		int row = ConsoleInput::lineNumber(static_cast<int>(records.size()));
		if (row == 0)
			return;
		CourseRecord record;
		record.account = ConsoleInput::text("学号:");
		if (!requireStudent(record.account))
			return;
		record.name = ConsoleInput::text("课程名:");
		record.credit = ConsoleInput::score(0.5f, 10.0f);
		record.grade = ConsoleInput::score(0.0f, 100.0f);
		service.updateCourse(row, record);
		markTotalNotGenerated(repository_);
		ConsoleView::message("修改成功!");
	}
	catch (const std::exception& ex) { ConsoleView::error(ex.what()); }
	ConsoleInput::pause();
}

void GroupConsole::deleteStudy()
{
	try
	{
		CourseService service(repository_);
		std::vector<CourseRecord> records = service.listAll();
		ConsoleView::header();
		ConsoleView::courses(records);
		int row = ConsoleInput::lineNumber(static_cast<int>(records.size()));
		if (row == 0)
			return;
		service.deleteCourse(row);
		markTotalNotGenerated(repository_);
		ConsoleView::message("删除成功!");
	}
	catch (const std::exception& ex) { ConsoleView::error(ex.what()); }
	ConsoleInput::pause();
}

void GroupConsole::additionMenu()
{
	ConsoleView::menu({ "录入附加分", "修改附加分", "删除附加分", "返回" });
	switch (ConsoleInput::menuChoice())
	{
	case '1': newAddition(); break;
	case '2': modifyAddition(); break;
	case '3': deleteAddition(); break;
	default: break;
	}
}

void GroupConsole::newAddition()
{
	try
	{
		AdditionService service(repository_);
		std::string mode = ConsoleInput::text("1:录入集体项目 2:录入个人项目 其他:返回\n请选择:");
		if (mode == "1")
			service.createGroupAddition(ConsoleInput::text("项目名称:"), ConsoleInput::score(0.5f, 5.0f));
		else if (mode == "2")
		{
			std::string account = ConsoleInput::text("学号:");
			if (!requireStudent(account))
				return;
			service.createPersonalAddition(account, ConsoleInput::text("项目名称:"), ConsoleInput::score(0.5f, 5.0f));
		}
		markTotalNotGenerated(repository_);
		ConsoleView::message("录入成功!");
	}
	catch (const std::exception& ex) { ConsoleView::error(ex.what()); }
	ConsoleInput::pause();
}

void GroupConsole::modifyAddition()
{
	try
	{
		AdditionService service(repository_);
		std::vector<ActivityRecord> records = service.listAll();
		ConsoleView::header();
		ConsoleView::activities(records);
		int row = ConsoleInput::lineNumber(static_cast<int>(records.size()));
		if (row == 0)
			return;
		ActivityRecord record;
		record.account = ConsoleInput::text("学号:");
		if (!requireStudent(record.account))
			return;
		record.name = ConsoleInput::text("项目名称:");
		record.grade = ConsoleInput::score(0.5f, 5.0f);
		service.updateAddition(row, record);
		markTotalNotGenerated(repository_);
		ConsoleView::message("修改成功!");
	}
	catch (const std::exception& ex) { ConsoleView::error(ex.what()); }
	ConsoleInput::pause();
}

void GroupConsole::deleteAddition()
{
	try
	{
		AdditionService service(repository_);
		std::vector<ActivityRecord> records = service.listAll();
		ConsoleView::header();
		ConsoleView::activities(records);
		int row = ConsoleInput::lineNumber(static_cast<int>(records.size()));
		if (row == 0)
			return;
		service.deleteAddition(row);
		markTotalNotGenerated(repository_);
		ConsoleView::message("删除成功!");
	}
	catch (const std::exception& ex) { ConsoleView::error(ex.what()); }
	ConsoleInput::pause();
}

void GroupConsole::checkActivity()
{
	try
	{
		ActivityService service(repository_);
		std::vector<ActivityRecord> records = service.listAll();
		ConsoleView::header();
		ConsoleView::activities(records);
		int row = ConsoleInput::lineNumber(static_cast<int>(records.size()));
		if (row == 0)
			return;
		char choice = ConsoleInput::text("1:审核通过  2:审核未通过\n请选择:")[0];
		if (choice == '1')
			service.approveActivity(row);
		else if (choice == '2')
			service.rejectActivity(row);
		else
			throw std::runtime_error("您的输入有误!");
		markTotalNotGenerated(repository_);
		ConsoleView::message("操作成功");
	}
	catch (const std::exception& ex) { ConsoleView::error(ex.what()); }
	ConsoleInput::pause();
}

void GroupConsole::searchMenu()
{
	try
	{
		QueryService query(repository_);
		ConsoleView::header();
		ConsoleView::scores(query.allScores());
	}
	catch (const std::exception& ex) { ConsoleView::error(ex.what()); }
	ConsoleInput::pause();
}

void GroupConsole::buildTotal()
{
	try
	{
		ScoreService service(repository_);
		TotalBuildResult result = service.validateBeforeBuild();
		if (!result.ready)
		{
			for (std::vector<std::string>::const_iterator iter = result.errors.begin(); iter != result.errors.end(); ++iter)
				ConsoleView::message(*iter);
			throw std::runtime_error("请先完成以上项目，再生成综测总分!");
		}
		service.buildTotal();
		repository_.users()[user_.account].finishedMoralOrGeneratedTotal = true;
		repository_.saveUsers();
		ConsoleView::message("生成成功！");
	}
	catch (const std::exception& ex) { ConsoleView::error(ex.what()); }
	ConsoleInput::pause();
}

void GroupConsole::changePassword()
{
	try
	{
		AuthService(repository_).changePassword(user_.account, ConsoleInput::text("请输入原密码:"), ConsoleInput::text("请输入新密码:"));
		ConsoleView::message("修改成功!");
	}
	catch (const std::exception& ex) { ConsoleView::error(ex.what()); }
	ConsoleInput::pause();
}

bool GroupConsole::requireStudent(const std::string& account) const
{
	std::map<std::string, UserRecord>::const_iterator iter = repository_.users().find(account);
	if (iter == repository_.users().end() || iter->second.role != UserRole::Student)
	{
		ConsoleView::message("没有学号为" + account + "的学生!");
		return false;
	}
	return true;
}
