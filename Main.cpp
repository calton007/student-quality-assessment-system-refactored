#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include "User.h"

#include <cstdlib>
#include <iomanip>
#include <iostream>

int main()
{
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
	std::cout.flags(std::ios::left);//设置输出格式为左对齐
	std::system("mode con cols=100 lines=25");//设置窗口大小
	SetConsoleTitleW(L"班级综合素质测评管理系统");//设置窗口标题
	User temp;//创建临时对象
	temp.Menu(temp.Login());//调用对象的登陆函数，判断登陆用户类型，创建对应的对象
	return 0;
}
