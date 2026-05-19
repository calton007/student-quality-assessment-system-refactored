#pragma once

#include <set>
#include <string>

struct AssessmentStatus
{
	bool totalGenerated = false;
	std::set<std::string> studentMoralFinishedAccounts;
	std::set<std::string> teacherMoralFinishedAccounts;
};

