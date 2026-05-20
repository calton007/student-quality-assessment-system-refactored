#pragma once

#include "ActivityRecord.h"
#include "AssessmentRepository.h"
#include "CourseRecord.h"
#include "MoralRecord.h"
#include "StudentScore.h"
#include "UserRecord.h"

#include <string>
#include <vector>

struct ScoreDetail
{
	UserRecord user;
	StudentScore score;
	std::vector<CourseRecord> courses;
	std::vector<ActivityRecord> activities;
	std::vector<ActivityRecord> additions;
	std::vector<MoralRecord> studentMorals;
	std::vector<MoralRecord> teacherMorals;
};

class ScoreDetailService
{
public:
	explicit ScoreDetailService(const AssessmentRepository& repository);

	ScoreDetail detailFor(const std::string& account) const;

private:
	const AssessmentRepository& repository_;
};
