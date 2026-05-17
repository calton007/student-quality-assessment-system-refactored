#pragma once

#include "AssessmentRepository.h"
#include "MoralRecord.h"

#include <string>
#include <vector>

class MoralService
{
public:
	explicit MoralService(AssessmentRepository& repository);

	static float total(const MoralRecord& record);
	std::vector<MoralRecord> studentMoralsGivenBy(const std::string& giverAccount) const;
	std::vector<MoralRecord> teacherMorals() const;
	void submitStudentMoral(const std::string& giverAccount, const std::vector<MoralRecord>& records);
	void submitTeacherMoral(const std::string& teacherAccount, const std::vector<MoralRecord>& records);
	void updateStudentMoral(const std::string& giverAccount, int row, const MoralRecord& record);
	void updateTeacherMoral(int row, const MoralRecord& record);

private:
	std::multimap<std::string, MoralRecord>::iterator findStudentMoralByGiverRow(const std::string& giverAccount, int row);
	std::map<std::string, MoralRecord>::iterator findTeacherMoralByRow(int row);

	AssessmentRepository& repository_;
};
