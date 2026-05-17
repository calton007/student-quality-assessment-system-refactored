#pragma once

#include "ActivityRecord.h"
#include "CourseRecord.h"
#include "MoralRecord.h"
#include "StudentScore.h"

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
	static void courses(const std::vector<CourseRecord>& records);
	static void activities(const std::vector<ActivityRecord>& records);
	static void morals(const std::vector<MoralRecord>& records);
	static void scores(const std::vector<StudentScore>& records);
};
