#include "TotalValidation.h"

namespace TotalValidation
{
	std::vector<std::string> validateReadiness(
		const std::vector<StudentReadiness>& students,
		int studentCount,
		const std::vector<std::string>& unreviewedActivityAccounts)
	{
		std::vector<std::string> errors;
		for (std::vector<StudentReadiness>::const_iterator iter = students.begin(); iter != students.end(); ++iter)
		{
			const std::string displayName = iter->name.empty() ? iter->account : iter->name;
			if (!iter->moralFinished)
				errors.push_back(displayName + "尚未完成思想品德打分");
			if (!iter->hasTotalRecord)
				errors.push_back(displayName + "缺少综测总表记录");
			if (!iter->hasCourse)
				errors.push_back(displayName + "缺少课程成绩");
			if (!iter->hasTeacherMoral)
				errors.push_back(displayName + "缺少班主任思想品德评分");
			if (iter->studentMoralCount < studentCount)
				errors.push_back(displayName + "学生互评记录缺失");
			if (iter->studentMoralCount > studentCount)
				errors.push_back(displayName + "学生互评记录重复");
		}

		for (std::vector<std::string>::const_iterator iter = unreviewedActivityAccounts.begin(); iter != unreviewedActivityAccounts.end(); ++iter)
			errors.push_back(*iter + "存在未审核的课外活动");
		return errors;
	}
}
