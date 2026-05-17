#include "AssessmentRepository.h"

#include "TextTable.h"

#include <fstream>
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

	std::ofstream openOutput(const std::string& path)
	{
		std::ofstream output(path);
		if (!output)
			throw std::runtime_error("cannot write data file: " + path);
		return output;
	}
}

AssessmentRepository::AssessmentRepository(const std::string& dataDirectory)
	: dataDirectory_(dataDirectory)
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
	std::ifstream input = openInput(path("User.txt"));
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
	std::ifstream input = openInput(path("Course.txt"));
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
	std::ifstream input = openInput(path("Act.txt"));
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
	std::ifstream input = openInput(path("Add.txt"));
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
	std::ifstream input = openInput(path("Moral_s.txt"));
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
	std::ifstream input = openInput(path("Moral_t.txt"));
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
	std::ifstream input = openInput(path("Total.txt"));
	const int count = TextTable::readRecordCount(input);
	for (int index = 0; index < count; ++index)
	{
		StudentScore record;
		requireRead(input, record, "Total.txt");
		if (!totals_.insert(std::pair<std::string, StudentScore>(record.account, record)).second)
			throw std::runtime_error("duplicate total account: " + record.account);
	}
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
	std::ofstream output = openOutput(path("User.txt"));
	output << users_.size() << '\n';
	for (std::map<std::string, UserRecord>::const_iterator iter = users_.begin(); iter != users_.end(); ++iter)
		output << iter->second;
}

void AssessmentRepository::saveCourses() const
{
	std::ofstream output = openOutput(path("Course.txt"));
	output << courses_.size() << '\n';
	for (std::multimap<std::string, CourseRecord>::const_iterator iter = courses_.begin(); iter != courses_.end(); ++iter)
		output << iter->second;
}

void AssessmentRepository::saveActivities() const
{
	std::ofstream output = openOutput(path("Act.txt"));
	output << activities_.size() << '\n';
	for (std::multimap<std::string, ActivityRecord>::const_iterator iter = activities_.begin(); iter != activities_.end(); ++iter)
		output << iter->second;
}

void AssessmentRepository::saveAdditions() const
{
	std::ofstream output = openOutput(path("Add.txt"));
	output << additions_.size() << '\n';
	for (std::multimap<std::string, ActivityRecord>::const_iterator iter = additions_.begin(); iter != additions_.end(); ++iter)
		output << iter->second;
}

void AssessmentRepository::saveStudentMorals() const
{
	std::ofstream output = openOutput(path("Moral_s.txt"));
	output << studentMoralsByReceiver_.size() << '\n';
	for (std::multimap<std::string, MoralRecord>::const_iterator iter = studentMoralsByReceiver_.begin(); iter != studentMoralsByReceiver_.end(); ++iter)
		writeMoralRecord(output, iter->second);
}

void AssessmentRepository::saveTeacherMorals() const
{
	std::ofstream output = openOutput(path("Moral_t.txt"));
	output << teacherMorals_.size() << '\n';
	for (std::map<std::string, MoralRecord>::const_iterator iter = teacherMorals_.begin(); iter != teacherMorals_.end(); ++iter)
		writeMoralRecord(output, iter->second);
}

void AssessmentRepository::saveTotals() const
{
	std::ofstream output = openOutput(path("Total.txt"));
	output << totals_.size() << '\n';
	for (std::map<std::string, StudentScore>::const_iterator iter = totals_.begin(); iter != totals_.end(); ++iter)
		output << iter->second;
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

std::string AssessmentRepository::path(const std::string& fileName) const
{
	if (dataDirectory_.empty() || dataDirectory_ == ".")
		return fileName;
	const char last = dataDirectory_[dataDirectory_.size() - 1];
	if (last == '\\' || last == '/')
		return dataDirectory_ + fileName;
	return dataDirectory_ + "\\" + fileName;
}
