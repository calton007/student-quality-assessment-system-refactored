#pragma once

#include "AssessmentRepository.h"

#include <string>
#include <vector>

class ActivityService
{
public:
	explicit ActivityService(AssessmentRepository& repository);

	std::vector<ActivityRecord> listForStudent(const std::string& account) const;
	std::vector<ActivityRecord> listAll() const;
	void createActivity(const std::string& account, const std::string& name, float grade);
	void updateStudentActivity(const std::string& account, int row, const std::string& name, float grade);
	void deleteStudentActivity(const std::string& account, int row);
	void approveActivity(int row);
	void rejectActivity(int row);

private:
	std::multimap<std::string, ActivityRecord>::iterator findStudentActivity(const std::string& account, int row);
	std::multimap<std::string, ActivityRecord>::iterator findActivityByGlobalRow(int row);

	AssessmentRepository& repository_;
};
