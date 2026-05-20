#pragma once

#include "AssessmentRepository.h"
#include "TotalValidation.h"

#include <string>
#include <vector>

struct TotalBuildResult
{
	bool ready;
	std::vector<std::string> errors;
	std::vector<TotalValidation::StudentReadiness> students;
	std::vector<std::string> pendingActivityAccounts;
};

class ScoreService
{
public:
	explicit ScoreService(AssessmentRepository& repository);

	TotalBuildResult validateBeforeBuild() const;
	void buildTotal();
	void revokeTotal();

private:
	void ensureTotalRecords();
	void buildMoral();
	void buildActivity();
	void buildAddition();
	void buildGpaAndStudy();
	void clearTotals();

	AssessmentRepository& repository_;
};
