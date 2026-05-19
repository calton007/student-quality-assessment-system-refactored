#include "ConsoleScreen.h"

#include "ConsoleTheme.h"

#include <cstdlib>
#include <iostream>

void ConsoleScreen::clear()
{
	std::system("cls");
}

void ConsoleScreen::header()
{
	header("", NULL);
}

void ConsoleScreen::header(const std::string& location, const UserRecord* user)
{
	clear();
	std::cout << ConsoleTheme::separator() << std::endl
		<< "**                                                 **" << std::endl
		<< "**         欢迎进入" << ConsoleTheme::title() << "        **" << std::endl
		<< "**                                                 **" << std::endl
		<< ConsoleTheme::separator() << std::endl;
	if (!location.empty())
		std::cout << "当前位置: " << location << std::endl;
	if (user != NULL)
		std::cout << "当前用户: " << user->name << "(" << user->account << ")" << std::endl;
}
