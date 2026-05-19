#include "ConsoleTheme.h"

const std::string& ConsoleTheme::title()
{
	static const std::string value = "班级综合素质测评管理系统";
	return value;
}

const std::string& ConsoleTheme::separator()
{
	static const std::string value = "*****************************************************";
	return value;
}

const std::string& ConsoleTheme::emptyData()
{
	static const std::string value = "暂无数据";
	return value;
}

const std::string& ConsoleTheme::invalidMenuChoice()
{
	static const std::string value = "请输入正确的菜单编号!";
	return value;
}

const std::string& ConsoleTheme::invalidLineNumber()
{
	static const std::string value = "请输入正确的行数!";
	return value;
}
