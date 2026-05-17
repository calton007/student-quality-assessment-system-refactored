#pragma once

#include "ActivityRecord.h"
#include "CourseRecord.h"
#include "MoralRecord.h"
#include "StudentScore.h"
#include "UserRecord.h"

#include <map>
#include <string>
#include <vector>

class AssessmentRepository
{
public:
	explicit AssessmentRepository(const std::string& dataDirectory = ".");

	void loadAll();
	void loadUsers();
	void loadCourses();
	void loadActivities();
	void loadAdditions();
	void loadStudentMorals();
	void loadTeacherMorals();
	void loadTotals();

	void saveUsers() const;
	void saveCourses() const;
	void saveActivities() const;
	void saveAdditions() const;
	void saveStudentMorals() const;
	void saveTeacherMorals() const;
	void saveTotals() const;

	std::map<std::string, UserRecord>& users();
	const std::map<std::string, UserRecord>& users() const;
	std::map<std::string, StudentScore>& totals();
	const std::map<std::string, StudentScore>& totals() const;
	std::multimap<std::string, CourseRecord>& courses();
	const std::multimap<std::string, CourseRecord>& courses() const;
	std::multimap<std::string, ActivityRecord>& activities();
	const std::multimap<std::string, ActivityRecord>& activities() const;
	std::multimap<std::string, ActivityRecord>& additions();
	const std::multimap<std::string, ActivityRecord>& additions() const;
	std::multimap<std::string, MoralRecord>& studentMoralsByReceiver();
	const std::multimap<std::string, MoralRecord>& studentMoralsByReceiver() const;
	std::multimap<std::string, MoralRecord>& studentMoralsByGiver();
	const std::multimap<std::string, MoralRecord>& studentMoralsByGiver() const;
	std::map<std::string, MoralRecord>& teacherMorals();
	const std::map<std::string, MoralRecord>& teacherMorals() const;

private:
	std::string path(const std::string& fileName) const;

	std::string dataDirectory_;
	std::map<std::string, UserRecord> users_;
	std::map<std::string, StudentScore> totals_;
	std::multimap<std::string, CourseRecord> courses_;
	std::multimap<std::string, ActivityRecord> activities_;
	std::multimap<std::string, ActivityRecord> additions_;
	std::multimap<std::string, MoralRecord> studentMoralsByReceiver_;
	std::multimap<std::string, MoralRecord> studentMoralsByGiver_;
	std::map<std::string, MoralRecord> teacherMorals_;
};
