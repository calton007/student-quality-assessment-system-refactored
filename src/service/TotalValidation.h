#pragma once

#include <string>
#include <vector>

namespace TotalValidation
{
	struct StudentReadiness
	{
		std::string account;
		std::string name;
		bool moralFinished;
		bool hasTotalRecord;
		bool hasCourse;
		bool hasTeacherMoral;
		int studentMoralCount;
	};

	std::vector<std::string> validateReadiness(
		const std::vector<StudentReadiness>& students,
		int studentCount,
		const std::vector<std::string>& unreviewedActivityAccounts);
}
