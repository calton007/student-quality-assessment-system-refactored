#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include "ConsoleApp.h"
#include "AuthService.h"
#include "ConsoleInput.h"
#include "ConsoleView.h"
#include "GroupConsole.h"
#include "StudentConsole.h"
#include "TeacherConsole.h"

#include <cstdlib>
#include <iomanip>
#include <iostream>

ConsoleApp::ConsoleApp(AssessmentRepository& repository)
	: repository_(repository)
{
}

int ConsoleApp::run()
{
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
	std::cout.flags(std::ios::left);
	std::system("mode con cols=100 lines=25");
	SetConsoleTitleW(L"班级综合素质测评管理系统");

	repository_.loadAll();
	while (true)
	{
		try
		{
			openMenu(login());
		}
		catch (const std::exception& ex)
		{
			ConsoleView::error(ex.what());
			ConsoleInput::pause();
		}
	}
	return 0;
}

UserRecord ConsoleApp::login()
{
	AuthService auth(repository_);
	while (true)
	{
		ConsoleView::header();
		const std::string account = ConsoleInput::text("账号:");
		const std::string password = ConsoleInput::text("密码:");
		try
		{
			UserRecord user = auth.login(account, password);
			ConsoleView::message("登陆成功！");
			ConsoleInput::pause();
			return user;
		}
		catch (const std::exception&)
		{
			ConsoleView::message("用户名或密码错误！");
		}
	}
}

void ConsoleApp::openMenu(const UserRecord& user)
{
	if (user.role == UserRole::Student)
	{
		StudentConsole(repository_, user).run();
		return;
	}
	if (user.role == UserRole::Teacher)
	{
		TeacherConsole(repository_, user).run();
		return;
	}
	GroupConsole(repository_, user).run();
}
