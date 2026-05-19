#include "AssessmentRepository.h"

#include "SecureFileStore.h"
#include "TextTable.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace
{
	template <typename Record>
	void requireRead(std::istream& in, Record& record, const std::string& fileName)
	{
		if (!(in >> record))
			throw std::runtime_error("invalid data file: " + fileName);
	}

	std::ifstream openInput(const std::string& path)
	{
		std::ifstream input(path);
		if (!input)
			throw std::runtime_error("cannot open data file: " + path);
		return input;
	}

	std::istringstream optionalRuntimeInput(const std::string& path, bool& loaded)
	{
		std::string content;
		loaded = SecureFileStore::readOptionalProtectedText(path, content);
		return std::istringstream(content);
	}
}

AssessmentRepository::AssessmentRepository(const std::string& dataDirectory)
	: userFilePath_(buildPath(dataDirectory, "User.txt")), runtimeDataDirectory_(dataDirectory), logger_(NULL)
{
}

AssessmentRepository::AssessmentRepository(const std::string& userFilePath, const std::string& runtimeDataDirectory, const AppLogger* logger)
	: userFilePath_(userFilePath), runtimeDataDirectory_(runtimeDataDirectory), logger_(logger)
{
}

void AssessmentRepository::loadAll()
{
	loadUsers();
	loadCourses();
	loadActivities();
	loadAdditions();
	loadStudentMorals();
	loadTeacherMorals();
	loadTotals();
}

void AssessmentRepository::loadUsers()
{
	users_.clear();
	std::ifstream input = openInput(userFilePath_);
	const int count = TextTable::readRecordCount(input);
	for (int index = 0; index < count; ++index)
	{
		UserRecord record;
		requireRead(input, record, "User.txt");
		if (!users_.insert(std::pair<std::string, UserRecord>(record.account, record)).second)
			throw std::runtime_error("duplicate user account: " + record.account);
	}
	TextTable::requireNoExtraRecords(input);
}

void AssessmentRepository::loadCourses()
{
	courses_.clear();
	bool loaded = false;
	std::istringstream input = optionalRuntimeInput(runtimePath("Course.txt"), loaded);
	if (!loaded)
		return;
	const int count = TextTable::readRecordCount(input);
	for (int index = 0; index < count; ++index)
	{
		CourseRecord record;
		requireRead(input, record, "Course.txt");
		courses_.insert(std::pair<std::string, CourseRecord>(record.account, record));
	}
	TextTable::requireNoExtraRecords(input);
}

void AssessmentRepository::loadActivities()
{
	activities_.clear();
	bool loaded = false;
	std::istringstream input = optionalRuntimeInput(runtimePath("Act.txt"), loaded);
	if (!loaded)
		return;
	const int count = TextTable::readRecordCount(input);
	for (int index = 0; index < count; ++index)
	{
		ActivityRecord record;
		requireRead(input, record, "Act.txt");
		activities_.insert(std::pair<std::string, ActivityRecord>(record.account, record));
	}
	TextTable::requireNoExtraRecords(input);
}

void AssessmentRepository::loadAdditions()
{
	additions_.clear();
	bool loaded = false;
	std::istringstream input = optionalRuntimeInput(runtimePath("Add.txt"), loaded);
	if (!loaded)
		return;
	const int count = TextTable::readRecordCount(input);
	for (int index = 0; index < count; ++index)
	{
		ActivityRecord record;
		requireRead(input, record, "Add.txt");
		if (record.status != ReviewStatus::Approved)
			throw std::runtime_error("addition must be approved");
		additions_.insert(std::pair<std::string, ActivityRecord>(record.account, record));
	}
	TextTable::requireNoExtraRecords(input);
}

void AssessmentRepository::loadStudentMorals()
{
	studentMoralsByReceiver_.clear();
	studentMoralsByGiver_.clear();
	bool loaded = false;
	std::istringstream input = optionalRuntimeInput(runtimePath("Moral_s.txt"), loaded);
	if (!loaded)
		return;
	const int count = TextTable::readRecordCount(input);
	for (int index = 0; index < count; ++index)
	{
		MoralRecord record;
		if (!readMoralRecord(input, record, 9))
			throw std::runtime_error("invalid data file: Moral_s.txt");
		studentMoralsByReceiver_.insert(std::pair<std::string, MoralRecord>(record.receiverAccount, record));
		studentMoralsByGiver_.insert(std::pair<std::string, MoralRecord>(record.giverAccount, record));
	}
	TextTable::requireNoExtraRecords(input);
}

void AssessmentRepository::loadTeacherMorals()
{
	teacherMorals_.clear();
	bool loaded = false;
	std::istringstream input = optionalRuntimeInput(runtimePath("Moral_t.txt"), loaded);
	if (!loaded)
		return;
	const int count = TextTable::readRecordCount(input);
	for (int index = 0; index < count; ++index)
	{
		MoralRecord record;
		if (!readMoralRecord(input, record, 3))
			throw std::runtime_error("invalid data file: Moral_t.txt");
		teacherMorals_.insert(std::pair<std::string, MoralRecord>(record.receiverAccount, record));
	}
	TextTable::requireNoExtraRecords(input);
}

void AssessmentRepository::loadTotals()
{
	totals_.clear();
	bool loaded = false;
	std::istringstream input = optionalRuntimeInput(runtimePath("Total.txt"), loaded);
	int count = 0;
	if (loaded)
		count = TextTable::readRecordCount(input);
	for (int index = 0; index < count; ++index)
	{
		StudentScore record;
		requireRead(input, record, "Total.txt");
		if (!totals_.insert(std::pair<std::string, StudentScore>(record.account, record)).second)
			throw std::runtime_error("duplicate total account: " + record.account);
	}
	if (loaded)
		TextTable::requireNoExtraRecords(input);
	if (count == 0)
	{
		for (std::map<std::string, UserRecord>::const_iterator iter = users_.begin(); iter != users_.end(); ++iter)
		{
			if (iter->second.role == UserRole::Student)
			{
				StudentScore score;
				score.account = iter->first;
				totals_.insert(std::pair<std::string, StudentScore>(iter->first, score));
			}
		}
	}
}

void AssessmentRepository::saveUsers() const
{
	std::ostringstream output;
	output << users_.size() << '\n';
	for (std::map<std::string, UserRecord>::const_iterator iter = users_.begin(); iter != users_.end(); ++iter)
		output << iter->second;
	writePlainUserFile(output.str());
}

void AssessmentRepository::saveCourses() const
{
	std::ostringstream output;
	output << courses_.size() << '\n';
	for (std::multimap<std::string, CourseRecord>::const_iterator iter = courses_.begin(); iter != courses_.end(); ++iter)
		output << iter->second;
	writeProtectedRuntimeFile("Course.txt", output.str());
}

void AssessmentRepository::saveActivities() const
{
	std::ostringstream output;
	output << activities_.size() << '\n';
	for (std::multimap<std::string, ActivityRecord>::const_iterator iter = activities_.begin(); iter != activities_.end(); ++iter)
		output << iter->second;
	writeProtectedRuntimeFile("Act.txt", output.str());
}

void AssessmentRepository::saveAdditions() const
{
	std::ostringstream output;
	output << additions_.size() << '\n';
	for (std::multimap<std::string, ActivityRecord>::const_iterator iter = additions_.begin(); iter != additions_.end(); ++iter)
		output << iter->second;
	writeProtectedRuntimeFile("Add.txt", output.str());
}

void AssessmentRepository::saveStudentMorals() const
{
	std::ostringstream output;
	output << studentMoralsByReceiver_.size() << '\n';
	for (std::multimap<std::string, MoralRecord>::const_iterator iter = studentMoralsByReceiver_.begin(); iter != studentMoralsByReceiver_.end(); ++iter)
		writeMoralRecord(output, iter->second);
	writeProtectedRuntimeFile("Moral_s.txt", output.str());
}

void AssessmentRepository::saveTeacherMorals() const
{
	std::ostringstream output;
	output << teacherMorals_.size() << '\n';
	for (std::map<std::string, MoralRecord>::const_iterator iter = teacherMorals_.begin(); iter != teacherMorals_.end(); ++iter)
		writeMoralRecord(output, iter->second);
	writeProtectedRuntimeFile("Moral_t.txt", output.str());
}

void AssessmentRepository::saveTotals() const
{
	std::ostringstream output;
	output << totals_.size() << '\n';
	for (std::map<std::string, StudentScore>::const_iterator iter = totals_.begin(); iter != totals_.end(); ++iter)
		output << iter->second;
	writeProtectedRuntimeFile("Total.txt", output.str());
}

std::map<std::string, UserRecord>& AssessmentRepository::users() { return users_; }
const std::map<std::string, UserRecord>& AssessmentRepository::users() const { return users_; }
std::map<std::string, StudentScore>& AssessmentRepository::totals() { return totals_; }
const std::map<std::string, StudentScore>& AssessmentRepository::totals() const { return totals_; }
std::multimap<std::string, CourseRecord>& AssessmentRepository::courses() { return courses_; }
const std::multimap<std::string, CourseRecord>& AssessmentRepository::courses() const { return courses_; }
std::multimap<std::string, ActivityRecord>& AssessmentRepository::activities() { return activities_; }
const std::multimap<std::string, ActivityRecord>& AssessmentRepository::activities() const { return activities_; }
std::multimap<std::string, ActivityRecord>& AssessmentRepository::additions() { return additions_; }
const std::multimap<std::string, ActivityRecord>& AssessmentRepository::additions() const { return additions_; }
std::multimap<std::string, MoralRecord>& AssessmentRepository::studentMoralsByReceiver() { return studentMoralsByReceiver_; }
const std::multimap<std::string, MoralRecord>& AssessmentRepository::studentMoralsByReceiver() const { return studentMoralsByReceiver_; }
std::multimap<std::string, MoralRecord>& AssessmentRepository::studentMoralsByGiver() { return studentMoralsByGiver_; }
const std::multimap<std::string, MoralRecord>& AssessmentRepository::studentMoralsByGiver() const { return studentMoralsByGiver_; }
std::map<std::string, MoralRecord>& AssessmentRepository::teacherMorals() { return teacherMorals_; }
const std::map<std::string, MoralRecord>& AssessmentRepository::teacherMorals() const { return teacherMorals_; }

std::string AssessmentRepository::buildPath(const std::string& directory, const std::string& fileName) const
{
	if (directory.empty() || directory == ".")
		return fileName;
	const char last = directory[directory.size() - 1];
	if (last == '\\' || last == '/')
		return directory + fileName;
	return directory + "\\" + fileName;
}

std::string AssessmentRepository::runtimePath(const std::string& fileName) const
{
	return buildPath(runtimeDataDirectory_, fileName);
}

void AssessmentRepository::writeProtectedRuntimeFile(const std::string& fileName, const std::string& content) const
{
	const std::string target = runtimePath(fileName);
	try
	{
		SecureFileStore::writeProtectedText(target, content);
	}
	catch (const std::exception& ex)
	{
		if (logger_ != NULL)
			logger_->error("save failed: " + target + ": " + ex.what());
		throw;
	}
}

void AssessmentRepository::writePlainUserFile(const std::string& content) const
{
	try
	{
		SecureFileStore::writePlainText(userFilePath_, content);
	}
	catch (const std::exception& ex)
	{
		if (logger_ != NULL)
			logger_->error("save failed: " + userFilePath_ + ": " + ex.what());
		throw;
	}
}
