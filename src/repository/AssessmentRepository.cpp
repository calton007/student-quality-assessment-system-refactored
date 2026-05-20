#include "AssessmentRepository.h"

#include "SecureFileStore.h"
#include "TextTable.h"

#include <fstream>
#include <filesystem>
#include <set>
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

	bool readOptionalToken(std::istream& input, std::string& value)
	{
		if (input >> value)
			return true;
		if (input.eof())
			return false;
		throw std::runtime_error("invalid data file");
	}

	bool flagFromFileValue(char value)
	{
		if (value == '0')
			return false;
		if (value == '1')
			return true;
		throw std::runtime_error("invalid flag");
	}

	std::string moralPairKey(const MoralRecord& record)
	{
		return record.receiverAccount + "\t" + record.giverAccount;
	}

	std::istringstream optionalRuntimeInput(const std::string& path, bool& loaded)
	{
		std::string content;
		loaded = SecureFileStore::readOptionalProtectedText(path, content);
		return std::istringstream(content);
	}

	void readStatusAccounts(std::istream& input, const std::string& label, std::set<std::string>& accounts)
	{
		std::string actualLabel;
		int count = 0;
		if (!(input >> actualLabel >> count) || actualLabel != label || count < 0)
			throw std::runtime_error("invalid data file: AssessmentStatus.txt");
		for (int index = 0; index < count; ++index)
		{
			std::string account;
			if (!(input >> account) || account.empty())
				throw std::runtime_error("invalid data file: AssessmentStatus.txt");
			if (!accounts.insert(account).second)
				throw std::runtime_error("duplicate status account: " + account);
		}
	}

	AssessmentStatus readAssessmentStatus(const std::string& content)
	{
		std::istringstream input(content);
		std::string header;
		std::string totalLabel;
		char totalValue = '0';
		AssessmentStatus status;
		if (!(input >> header) || header != "SQAS-STATUS-1")
			throw std::runtime_error("invalid data file: AssessmentStatus.txt");
		if (!(input >> totalLabel >> totalValue) || totalLabel != "total_generated")
			throw std::runtime_error("invalid data file: AssessmentStatus.txt");
		status.totalGenerated = flagFromFileValue(totalValue);
		readStatusAccounts(input, "student_moral_finished", status.studentMoralFinishedAccounts);
		readStatusAccounts(input, "teacher_moral_finished", status.teacherMoralFinishedAccounts);
		TextTable::requireNoExtraRecords(input);
		return status;
	}

	std::string writeAssessmentStatus(const AssessmentStatus& status)
	{
		std::ostringstream output;
		output << "SQAS-STATUS-1\n";
		output << "total_generated " << (status.totalGenerated ? '1' : '0') << '\n';
		output << "student_moral_finished " << status.studentMoralFinishedAccounts.size() << '\n';
		for (std::set<std::string>::const_iterator iter = status.studentMoralFinishedAccounts.begin(); iter != status.studentMoralFinishedAccounts.end(); ++iter)
			output << *iter << '\n';
		output << "teacher_moral_finished " << status.teacherMoralFinishedAccounts.size() << '\n';
		for (std::set<std::string>::const_iterator iter = status.teacherMoralFinishedAccounts.begin(); iter != status.teacherMoralFinishedAccounts.end(); ++iter)
			output << *iter << '\n';
		return output.str();
	}

	void readUserLine(const std::string& line, UserRecord& record, bool& hasLegacyFlag, bool& legacyFlag)
	{
		std::istringstream input(line);
		char role = '0';
		if (!(input >> record.account >> record.name >> record.password >> role))
			throw std::runtime_error("invalid data file: User.txt");
		record.role = userRoleFromFileValue(role);

		std::string flag;
		if (readOptionalToken(input, flag))
		{
			if (flag.size() != 1)
				throw std::runtime_error("invalid data file: User.txt");
			hasLegacyFlag = true;
			legacyFlag = flagFromFileValue(flag[0]);
		}
		else
		{
			hasLegacyFlag = false;
			legacyFlag = false;
		}

		std::string extra;
		if (readOptionalToken(input, extra))
			throw std::runtime_error("invalid data file: User.txt");
	}
}

AssessmentRepository::AssessmentRepository(const std::string& dataDirectory)
	: userFilePath_(buildPath(dataDirectory, "User.txt")), runtimeDataDirectory_(dataDirectory), logger_(NULL), hasPendingLegacyStatus_(false)
{
}

AssessmentRepository::AssessmentRepository(const std::string& userFilePath, const std::string& runtimeDataDirectory, const AppLogger* logger)
	: userFilePath_(userFilePath), runtimeDataDirectory_(runtimeDataDirectory), logger_(logger), hasPendingLegacyStatus_(false)
{
}

void AssessmentRepository::loadAll()
{
	loadUsers();
	loadStatus();
	loadCourses();
	loadActivities();
	loadAdditions();
	loadStudentMorals();
	loadTeacherMorals();
	loadTotals();
	finishLegacyMigration();
}

void AssessmentRepository::loadUsers()
{
	users_.clear();
	std::ifstream input = openInput(userFilePath_);
	const int count = TextTable::readRecordCount(input);
	pendingLegacyStatus_ = AssessmentStatus();
	hasPendingLegacyStatus_ = false;
	bool hasLegacyUsers = false;
	bool hasModernUsers = false;
	for (int index = 0; index < count; ++index)
	{
		std::string line;
		if (!(input >> std::ws) || !std::getline(input, line) || line.empty())
			throw std::runtime_error("invalid data file: User.txt");
		UserRecord record;
		bool hasLegacyFlag = false;
		bool legacyFlag = false;
		readUserLine(line, record, hasLegacyFlag, legacyFlag);
		hasLegacyUsers = hasLegacyUsers || hasLegacyFlag;
		hasModernUsers = hasModernUsers || !hasLegacyFlag;
		if (hasLegacyUsers && hasModernUsers)
			throw std::runtime_error("User.txt mixes old and new formats");
		if (!users_.insert(std::pair<std::string, UserRecord>(record.account, record)).second)
			throw std::runtime_error("duplicate user account: " + record.account);
		if (hasLegacyFlag && legacyFlag)
		{
			if (record.role == UserRole::Student)
				pendingLegacyStatus_.studentMoralFinishedAccounts.insert(record.account);
			else if (record.role == UserRole::Teacher)
				pendingLegacyStatus_.teacherMoralFinishedAccounts.insert(record.account);
			else if (record.role == UserRole::Group)
				pendingLegacyStatus_.totalGenerated = true;
		}
	}
	TextTable::requireNoExtraRecords(input);
	input.close();
	if (hasLegacyUsers)
		hasPendingLegacyStatus_ = true;
}

void AssessmentRepository::loadStatus()
{
	status_ = AssessmentStatus();
	bool loaded = false;
	std::string content;
	loaded = SecureFileStore::readOptionalProtectedText(runtimePath("AssessmentStatus.txt"), content);
	if (!loaded)
		return;
	status_ = readAssessmentStatus(content);
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
	std::set<std::string> uniquePairs;
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
		if (!uniquePairs.insert(moralPairKey(record)).second)
			throw std::runtime_error("学生互评记录重复: 打分人 " + record.giverAccount + " -> 被评分人 " + record.receiverAccount);
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

void AssessmentRepository::saveStatus() const
{
	writeProtectedRuntimeFile("AssessmentStatus.txt", writeAssessmentStatus(status_));
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
AssessmentStatus& AssessmentRepository::status() { return status_; }
const AssessmentStatus& AssessmentRepository::status() const { return status_; }
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
const std::string& AssessmentRepository::runtimeDataDirectory() const { return runtimeDataDirectory_; }

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

void AssessmentRepository::migrateLegacyUserStatus(const AssessmentStatus& legacyStatus)
{
	if (std::filesystem::exists(runtimePath("AssessmentStatus.txt")))
		throw std::runtime_error("cannot migrate legacy User.txt when AssessmentStatus.txt already exists");
	status_ = legacyStatus;
	saveStatus();
	saveUsers();
}

void AssessmentRepository::finishLegacyMigration()
{
	if (!hasPendingLegacyStatus_)
		return;
	validateLegacyStatus();
	migrateLegacyUserStatus(pendingLegacyStatus_);
	hasPendingLegacyStatus_ = false;
}

void AssessmentRepository::validateLegacyStatus() const
{
	for (std::set<std::string>::const_iterator account = pendingLegacyStatus_.studentMoralFinishedAccounts.begin(); account != pendingLegacyStatus_.studentMoralFinishedAccounts.end(); ++account)
	{
		if (studentMoralsByGiver_.find(*account) == studentMoralsByGiver_.end())
			throw std::runtime_error("legacy User.txt status conflicts with missing student moral records: " + *account);
	}
	for (std::set<std::string>::const_iterator account = pendingLegacyStatus_.teacherMoralFinishedAccounts.begin(); account != pendingLegacyStatus_.teacherMoralFinishedAccounts.end(); ++account)
	{
		bool found = false;
		for (std::map<std::string, MoralRecord>::const_iterator iter = teacherMorals_.begin(); iter != teacherMorals_.end(); ++iter)
		{
			if (iter->second.giverAccount == *account)
			{
				found = true;
				break;
			}
		}
		if (!found)
			throw std::runtime_error("legacy User.txt status conflicts with missing teacher moral records: " + *account);
	}
	if (pendingLegacyStatus_.totalGenerated)
	{
		for (std::map<std::string, StudentScore>::const_iterator iter = totals_.begin(); iter != totals_.end(); ++iter)
		{
			if (iter->second.rank > 0 || iter->second.total > 0.0f)
				return;
		}
		throw std::runtime_error("legacy User.txt status conflicts with missing total score records");
	}
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
