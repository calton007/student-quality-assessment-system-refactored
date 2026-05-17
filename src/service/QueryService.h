#pragma once

#include "AssessmentRepository.h"

#include <string>
#include <vector>

class QueryService
{
public:
	explicit QueryService(const AssessmentRepository& repository);

	const StudentScore& scoreFor(const std::string& account) const;
	std::vector<StudentScore> allScores() const;
	std::string userName(const std::string& account) const;
	bool isStudent(const std::string& account) const;
	bool totalGenerated() const;

private:
	const AssessmentRepository& repository_;
};
