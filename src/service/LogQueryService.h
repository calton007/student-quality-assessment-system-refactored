#pragma once

#include "AssessmentRepository.h"

#include <string>
#include <vector>

class LogQueryService
{
public:
	explicit LogQueryService(const AssessmentRepository& repository);

	std::vector<std::string> recent(const std::string& level, size_t limit) const;

private:
	std::string logFilePath() const;

	const AssessmentRepository& repository_;
};
