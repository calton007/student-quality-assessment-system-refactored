#pragma once

#include "AssessmentRepository.h"

#include <string>
#include <vector>

struct TotalBuildResult
{
	bool ready;
	std::vector<std::string> errors;
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
