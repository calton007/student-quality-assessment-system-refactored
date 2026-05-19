#pragma once

#include "ActivityRecord.h"
#include "CourseRecord.h"
#include "MoralRecord.h"
#include "StudentScore.h"
#include "UserRecord.h"

#include <string>
#include <vector>

class ConsoleView
{
public:
	static void clear();
	static void header();
	static void message(const std::string& value);
	static void error(const std::string& value);
	static void menu(const std::vector<std::string>& items);
	static void menu(const std::string& location, const UserRecord& user, const std::vector<std::string>& items);
	static void operation(const std::string& studentName, const std::string& studentAccount, const std::string& item, const std::string& action);
	static void courses(const std::vector<CourseRecord>& records);
	static void activities(const std::vector<ActivityRecord>& records);
	static void morals(const std::vector<MoralRecord>& records);
	static void scores(const std::vector<StudentScore>& records);
};
