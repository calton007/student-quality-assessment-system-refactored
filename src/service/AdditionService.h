#pragma once

#include "AssessmentRepository.h"

#include <string>
#include <vector>

class AdditionService
{
public:
	explicit AdditionService(AssessmentRepository& repository);

	std::vector<ActivityRecord> listAll() const;
	void createPersonalAddition(const std::string& account, const std::string& name, float grade);
	void createGroupAddition(const std::string& name, float grade);
	void updateAddition(int row, const ActivityRecord& replacement);
	void deleteAddition(int row);

private:
	std::multimap<std::string, ActivityRecord>::iterator findByGlobalRow(int row);

	AssessmentRepository& repository_;
};
