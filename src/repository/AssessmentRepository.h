#pragma once

#include "ActivityRecord.h"
#include "AppLogger.h"
#include "AssessmentStatus.h"
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
	AssessmentRepository(const std::string& userFilePath, const std::string& runtimeDataDirectory, const AppLogger* logger);

	void loadAll();
	void loadUsers();
	void loadStatus();
	void loadCourses();
	void loadActivities();
	void loadAdditions();
	void loadStudentMorals();
	void loadTeacherMorals();
	void loadTotals();

	void saveUsers() const;
	void saveStatus() const;
	void saveCourses() const;
	void saveActivities() const;
	void saveAdditions() const;
	void saveStudentMorals() const;
	void saveTeacherMorals() const;
	void saveTotals() const;

	std::map<std::string, UserRecord>& users();
	const std::map<std::string, UserRecord>& users() const;
	AssessmentStatus& status();
	const AssessmentStatus& status() const;
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
	std::string buildPath(const std::string& directory, const std::string& fileName) const;
	std::string runtimePath(const std::string& fileName) const;
	void finishLegacyMigration();
	void validateLegacyStatus() const;
	void migrateLegacyUserStatus(const AssessmentStatus& legacyStatus);
	void writeProtectedRuntimeFile(const std::string& fileName, const std::string& content) const;
	void writePlainUserFile(const std::string& content) const;

	std::string userFilePath_;
	std::string runtimeDataDirectory_;
	const AppLogger* logger_;
	std::map<std::string, UserRecord> users_;
	AssessmentStatus status_;
	bool hasPendingLegacyStatus_;
	AssessmentStatus pendingLegacyStatus_;
	std::map<std::string, StudentScore> totals_;
	std::multimap<std::string, CourseRecord> courses_;
	std::multimap<std::string, ActivityRecord> activities_;
	std::multimap<std::string, ActivityRecord> additions_;
	std::multimap<std::string, MoralRecord> studentMoralsByReceiver_;
	std::multimap<std::string, MoralRecord> studentMoralsByGiver_;
	std::map<std::string, MoralRecord> teacherMorals_;
};
