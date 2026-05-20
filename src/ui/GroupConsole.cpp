#include "GroupConsole.h"

#include "ActivityService.h"
#include "AdditionService.h"
#include "AuthService.h"
#include "ConsoleInput.h"
#include "ConsoleTable.h"
#include "ConsoleView.h"
#include "CourseService.h"
#include "ExportService.h"
#include "QueryService.h"
#include "ScoreDetailService.h"
#include "ScoreService.h"

#include <cstdlib>
#include <iostream>

namespace
{
	std::string readinessStatus(bool value)
	{
		return value ? "完成" : "缺失";
	}

	std::string countStatus(int actual, int expected)
	{
		if (actual == expected)
			return "完成";
		return std::to_string(actual) + "/" + std::to_string(expected);
	}

	void displayReadinessReport(const TotalBuildResult& result)
	{
		if (result.ready)
		{
			ConsoleView::message("生成条件已满足，可以生成综测成绩。");
			return;
		}

		const int studentCount = static_cast<int>(result.students.size());
		std::vector<std::vector<std::string>> rows;
		for (std::vector<TotalValidation::StudentReadiness>::const_iterator iter = result.students.begin(); iter != result.students.end(); ++iter)
		{
			std::vector<std::string> row;
			row.push_back(iter->account);
			row.push_back(iter->name);
			row.push_back(readinessStatus(iter->hasCourse));
			row.push_back(countStatus(iter->studentMoralCount, studentCount));
			row.push_back(readinessStatus(iter->hasTeacherMoral));
			row.push_back(readinessStatus(iter->moralFinished));
			rows.push_back(row);
		}
		ConsoleTable::render({
			ConsoleTable::Column("学号", 10),
			ConsoleTable::Column("姓名", 12),
			ConsoleTable::Column("课程成绩", 12),
			ConsoleTable::Column("学生互评", 12),
			ConsoleTable::Column("辅导员评分", 14),
			ConsoleTable::Column("本人互评", 12)
		}, rows);

		for (std::vector<std::string>::const_iterator iter = result.pendingActivityAccounts.begin(); iter != result.pendingActivityAccounts.end(); ++iter)
			ConsoleView::message(*iter + "存在未审核的课外活动");
	}

	void displayScoreDetail(const ScoreDetail& detail)
	{
		ConsoleView::message("学生: " + detail.user.name + "(" + detail.user.account + ")");
		ConsoleView::scores(std::vector<StudentScore>(1, detail.score));
		ConsoleView::message("课程成绩:");
		ConsoleView::courses(detail.courses);
		ConsoleView::message("课外活动:");
		ConsoleView::activities(detail.activities);
		ConsoleView::message("附加分:");
		ConsoleView::activities(detail.additions);
		ConsoleView::message("学生互评:");
		ConsoleView::morals(detail.studentMorals);
		ConsoleView::message("辅导员评分:");
		ConsoleView::morals(detail.teacherMorals);
	}
}

GroupConsole::GroupConsole(AssessmentRepository& repository, const UserRecord& user, const AppLogger& logger)
	: repository_(repository), user_(user), logger_(logger)
{
}

void GroupConsole::run()
{
	while (true)
	{
		const bool totalGenerated = repository_.status().totalGenerated;
		ConsoleView::menu("测评小组首页", user_, homeMenuItems(totalGenerated));
		if (totalGenerated)
		{
			switch (ConsoleInput::menuChoice(7))
			{
			case '1': searchMenu(); break;
			case '2': scoreDetail(); break;
			case '3': exportTotals(); break;
			case '4': buildTotal(); break;
			case '5': changePassword(); break;
			case '0':
			case '6': return;
			case '7': std::exit(0);
			default: ConsoleView::error("您的输入有误,请重新输入!"); ConsoleInput::pause(); break;
			}
		}
		else
		{
			switch (ConsoleInput::menuChoice(8))
			{
			case '1': studyMenu(); break;
			case '2': additionMenu(); break;
			case '3': checkActivity(); break;
			case '4': searchMenu(); break;
			case '5': buildTotal(); break;
			case '6': changePassword(); break;
			case '0':
			case '7': return;
			case '8': std::exit(0);
			default: ConsoleView::error("您的输入有误,请重新输入!"); ConsoleInput::pause(); break;
			}
		}
	}
}

std::vector<std::string> GroupConsole::homeMenuItems(bool totalGenerated)
{
	if (totalGenerated)
		return { "查询项目", "综测成绩详情", "导出综测成绩", "综测成绩生成", "修改密码", "返回登陆界面", "退出系统" };
	return { "学习成绩管理", "附加分管理", "审核课外活动加分", "查询项目", "综测成绩生成", "修改密码", "返回登陆界面", "退出系统" };
}

void GroupConsole::studyMenu()
{
	ConsoleView::menu("测评小组首页 / 学习成绩管理", user_, { "录入学习成绩", "修改学习成绩", "删除学习成绩", "返回" });
	switch (ConsoleInput::menuChoice(4))
	{
	case '1': newStudy(); break;
	case '2': modifyStudy(); break;
	case '3': deleteStudy(); break;
	case '0':
	default: break;
	}
}

void GroupConsole::newStudy()
{
	try
	{
		ConsoleView::menu("测评小组首页 / 学习成绩管理 / 录入学习成绩", user_, std::vector<std::string>());
		CourseService service(repository_);
		int mode = ConsoleInput::choice("1:批量录入成绩 2:单个录入成绩", 2);
		if (mode == 0)
			return;
		if (mode == 2)
		{
			std::string account = ConsoleInput::optionalText("录入学生的学号(返回请输入0):");
			if (account.empty())
				return;
			if (!requireStudent(account))
				return;
			ConsoleView::operation(repository_.users()[account].name, account, "学习成绩", "录入");
			std::string courseName = ConsoleInput::optionalText("课程名(返回请输入0):");
			if (courseName.empty())
				return;
			service.createCourse(account, courseName, ConsoleInput::score(0.5f, 10.0f, "学分"), ConsoleInput::score(0.0f, 100.0f, "成绩"));
		}
		else if (mode == 1)
		{
			std::string name = ConsoleInput::optionalText("课程名(返回请输入0):");
			if (name.empty())
				return;
			float credit = ConsoleInput::score(0.5f, 10.0f, "学分");
			for (std::map<std::string, UserRecord>::const_iterator iter = repository_.users().begin(); iter != repository_.users().end(); ++iter)
			{
				if (iter->second.role == UserRole::Student)
				{
					ConsoleView::operation(iter->second.name, iter->first, "学习成绩", "录入");
					service.createCourse(iter->first, name, credit, ConsoleInput::score(0.0f, 100.0f, "成绩"));
				}
			}
		}
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
		ConsoleView::menu("测评小组首页 / 学习成绩管理 / 修改学习成绩", user_, std::vector<std::string>());
		ConsoleView::courses(records);
		int row = ConsoleInput::lineNumber(static_cast<int>(records.size()));
		if (row == 0)
			return;
		CourseRecord record;
		record.account = ConsoleInput::optionalText("学号(返回请输入0):");
		if (record.account.empty())
			return;
		if (!requireStudent(record.account))
			return;
		ConsoleView::operation(repository_.users()[record.account].name, record.account, "学习成绩", "修改");
		record.name = ConsoleInput::optionalText("课程名(返回请输入0):");
		if (record.name.empty())
			return;
		record.credit = ConsoleInput::score(0.5f, 10.0f, "学分");
		record.grade = ConsoleInput::score(0.0f, 100.0f, "成绩");
		service.updateCourse(row, record);
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
		ConsoleView::menu("测评小组首页 / 学习成绩管理 / 删除学习成绩", user_, std::vector<std::string>());
		ConsoleView::courses(records);
		int row = ConsoleInput::lineNumber(static_cast<int>(records.size()));
		if (row == 0)
			return;
		ConsoleView::operation(repository_.users()[records[row - 1].account].name, records[row - 1].account, "学习成绩", "删除");
		service.deleteCourse(row);
		ConsoleView::message("删除成功!");
	}
	catch (const std::exception& ex) { ConsoleView::error(ex.what()); }
	ConsoleInput::pause();
}

void GroupConsole::additionMenu()
{
	ConsoleView::menu("测评小组首页 / 附加分管理", user_, { "录入附加分", "修改附加分", "删除附加分", "返回" });
	switch (ConsoleInput::menuChoice(4))
	{
	case '1': newAddition(); break;
	case '2': modifyAddition(); break;
	case '3': deleteAddition(); break;
	case '0':
	default: break;
	}
}

void GroupConsole::newAddition()
{
	try
	{
		ConsoleView::menu("测评小组首页 / 附加分管理 / 录入附加分", user_, std::vector<std::string>());
		AdditionService service(repository_);
		int mode = ConsoleInput::choice("1:录入集体项目 2:录入个人项目", 2);
		if (mode == 0)
			return;
		if (mode == 1)
		{
			std::string name = ConsoleInput::optionalText("项目名称(返回请输入0):");
			if (name.empty())
				return;
			ConsoleView::operation("全体学生", "ALL", "附加分", "录入");
			service.createGroupAddition(name, ConsoleInput::score(0.5f, 5.0f, "附加分"));
		}
		else if (mode == 2)
		{
			std::string account = ConsoleInput::optionalText("学号(返回请输入0):");
			if (account.empty())
				return;
			if (!requireStudent(account))
				return;
			ConsoleView::operation(repository_.users()[account].name, account, "附加分", "录入");
			std::string name = ConsoleInput::optionalText("项目名称(返回请输入0):");
			if (name.empty())
				return;
			service.createPersonalAddition(account, name, ConsoleInput::score(0.5f, 5.0f, "附加分"));
		}
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
		ConsoleView::menu("测评小组首页 / 附加分管理 / 修改附加分", user_, std::vector<std::string>());
		ConsoleView::activities(records);
		int row = ConsoleInput::lineNumber(static_cast<int>(records.size()));
		if (row == 0)
			return;
		ActivityRecord record;
		record.account = ConsoleInput::optionalText("学号(返回请输入0):");
		if (record.account.empty())
			return;
		if (!requireStudent(record.account))
			return;
		ConsoleView::operation(repository_.users()[record.account].name, record.account, "附加分", "修改");
		record.name = ConsoleInput::optionalText("项目名称(返回请输入0):");
		if (record.name.empty())
			return;
		record.grade = ConsoleInput::score(0.5f, 5.0f, "附加分");
		service.updateAddition(row, record);
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
		ConsoleView::menu("测评小组首页 / 附加分管理 / 删除附加分", user_, std::vector<std::string>());
		ConsoleView::activities(records);
		int row = ConsoleInput::lineNumber(static_cast<int>(records.size()));
		if (row == 0)
			return;
		ConsoleView::operation(repository_.users()[records[row - 1].account].name, records[row - 1].account, "附加分", "删除");
		service.deleteAddition(row);
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
		ConsoleView::menu("测评小组首页 / 审核课外活动加分", user_, std::vector<std::string>());
		ConsoleView::activities(records);
		int row = ConsoleInput::lineNumber(static_cast<int>(records.size()));
		if (row == 0)
			return;
		int choice = ConsoleInput::choice("1:审核通过 2:审核未通过", 2);
		if (choice == 0)
			return;
		ConsoleView::operation(repository_.users()[records[row - 1].account].name, records[row - 1].account, "课外活动", "审核");
		if (choice == 1)
			service.approveActivity(row);
		else if (choice == 2)
			service.rejectActivity(row);
		else
			throw std::runtime_error("您的输入有误,请重新输入!");
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
		if (!query.totalGenerated())
			throw std::runtime_error("尚未生成综测成绩,无法查询!");
		ConsoleView::menu("测评小组首页 / 查询项目", user_, std::vector<std::string>());
		ConsoleView::scores(query.allScores());
	}
	catch (const std::exception& ex) { ConsoleView::error(ex.what()); }
	ConsoleInput::pause();
}

void GroupConsole::scoreDetail()
{
	try
	{
		ConsoleView::menu("测评小组首页 / 综测成绩详情", user_, std::vector<std::string>());
		std::string account = ConsoleInput::optionalText("学号(返回请输入0):");
		if (account.empty())
			return;
		displayScoreDetail(ScoreDetailService(repository_).detailFor(account));
	}
	catch (const std::exception& ex) { ConsoleView::error(ex.what()); }
	ConsoleInput::pause();
}

void GroupConsole::exportTotals()
{
	try
	{
		ConsoleView::menu("测评小组首页 / 导出综测成绩", user_, std::vector<std::string>());
		const std::string path = ExportService(repository_).exportTotalsCsv();
		logger_.info("total exported by: " + user_.account);
		ConsoleView::message("导出成功: " + path);
	}
	catch (const std::exception& ex) { ConsoleView::error(ex.what()); }
	ConsoleInput::pause();
}

void GroupConsole::buildTotal()
{
	try
	{
		ScoreService service(repository_);
		if (repository_.status().totalGenerated)
		{
			ConsoleView::menu("测评小组首页 / 综测成绩生成", user_, { "撤销综测成绩", "返回" });
			const int choice = ConsoleInput::choice("请选择", 2);
			if (choice == 0 || choice == 2)
				return;
			service.revokeTotal();
			logger_.info("total revoked by: " + user_.account);
			ConsoleView::message("撤销成功！");
		}
		else
		{
			ConsoleView::menu("测评小组首页 / 综测成绩生成", user_, { "检查生成条件", "生成综测成绩", "返回" });
			const int choice = ConsoleInput::choice("请选择", 3);
			if (choice == 0 || choice == 3)
				return;
			TotalBuildResult result = service.validateBeforeBuild();
			if (choice == 1)
			{
				displayReadinessReport(result);
				return;
			}
			if (!result.ready)
			{
				displayReadinessReport(result);
				throw std::runtime_error("请先完成以上项目，再生成综测总分!");
			}
			service.buildTotal();
			logger_.info("total generated by: " + user_.account);
			ConsoleView::message("生成成功！");
		}
	}
	catch (const std::exception& ex) { ConsoleView::error(ex.what()); }
	ConsoleInput::pause();
}

void GroupConsole::changePassword()
{
	try
	{
		ConsoleView::menu("测评小组首页 / 修改密码", user_, std::vector<std::string>());
		AuthService(repository_).changePassword(user_.account, ConsoleInput::textRequired("请输入原密码:"), ConsoleInput::textRequired("请输入新密码:"));
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
		ConsoleView::error("没有学号为" + account + "的学生!");
		return false;
	}
	return true;
}
