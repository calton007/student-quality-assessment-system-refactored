#pragma once

#include "AssessmentRepository.h"

#include <string>

class ExportService
{
public:
	explicit ExportService(const AssessmentRepository& repository);

	std::string exportTotalsCsv() const;

private:
	std::string buildCsv() const;

	const AssessmentRepository& repository_;
};
