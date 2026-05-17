#include "ScoreCalculator.h"
#include "TextTable.h"
#include "TotalValidation.h"
#include "AssessmentRepository.h"
#include "ActivityService.h"
#include "AdditionService.h"
#include "AuthService.h"
#include "CourseService.h"
#include "ConsoleInput.h"
#include "ConsoleView.h"
#include "MoralService.h"
#include "PasswordHasher.h"
#include "QueryService.h"
#include "ScoreService.h"

#include <cassert>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace
{
	bool closeTo(float left, float right)
	{
		return std::fabs(left - right) < 0.001f;
	}

	void assertThrowsRecordError(const std::string& text)
	{
		bool thrown = false;
		try
		{
			std::stringstream input(text);
			TextTable::readRecordCount(input);
		}
		catch (const std::runtime_error&)
		{
			thrown = true;
		}
		assert(thrown);
	}

	void testTextTable()
	{
		std::stringstream good("2\n10002 90\n10003 80\n");
		const int lines = TextTable::readRecordCount(good);
		assert(lines == 2);
		std::string account;
		int grade = 0;
		assert(good >> account >> grade);
		assert(account == "10002");
		assert(grade == 90);
		assert(good >> account >> grade);
		TextTable::requireNoExtraRecords(good);

		std::stringstream extra("1\n10002 90\n10003 80\n");
		assert(TextTable::readRecordCount(extra) == 1);
		assert(extra >> account >> grade);
		bool extraThrown = false;
		try
		{
			TextTable::requireNoExtraRecords(extra);
		}
		catch (const std::runtime_error&)
		{
			extraThrown = true;
		}
		assert(extraThrown);

		std::stringstream missing("1\n10002\n");
		assert(TextTable::readRecordCount(missing) == 1);
		assert(!(missing >> account >> grade));

		assertThrowsRecordError("-1\n");
		assertThrowsRecordError("abc\n");
	}

	void testScoreCalculator()
	{
		assert(closeTo(ScoreCalculator::gradePoint(90), 4.0f));
		assert(closeTo(ScoreCalculator::gradePoint(85), 3.7f));
		assert(closeTo(ScoreCalculator::gradePoint(82), 3.3f));
		assert(closeTo(ScoreCalculator::gradePoint(78), 3.0f));
		assert(closeTo(ScoreCalculator::gradePoint(75), 2.7f));
		assert(closeTo(ScoreCalculator::gradePoint(72), 2.3f));
		assert(closeTo(ScoreCalculator::gradePoint(68), 2.0f));
		assert(closeTo(ScoreCalculator::gradePoint(64), 1.5f));
		assert(closeTo(ScoreCalculator::gradePoint(60), 1.0f));
		assert(closeTo(ScoreCalculator::gradePoint(59), 0.0f));

		std::vector<std::pair<float, float>> courses;
		courses.push_back(std::pair<float, float>(2.0f, 90.0f));
		courses.push_back(std::pair<float, float>(1.0f, 85.0f));
		assert(closeTo(ScoreCalculator::gpa(courses), 3.9f));
		assert(closeTo(ScoreCalculator::studyScore(3.99f), 99.75f));
		assert(closeTo(ScoreCalculator::cappedAddition(7.5f), 5.0f));
		assert(closeTo(ScoreCalculator::cappedAddition(4.5f), 4.5f));
		assert(closeTo(ScoreCalculator::totalScore(80.0f, 20.0f, 90.0f, 3.0f), 75.5f));
		assert(closeTo(ScoreCalculator::totalScore(100.0f, 100.0f, 100.0f, 20.0f), 100.0f));
	}

	void testRank()
	{
		std::vector<std::pair<std::string, float>> totals;
		totals.push_back(std::pair<std::string, float>("10002", 80.0f));
		totals.push_back(std::pair<std::string, float>("10003", 90.0f));
		totals.push_back(std::pair<std::string, float>("10004", 70.0f));

		std::vector<std::pair<std::string, int>> ranks = ScoreCalculator::rankByTotal(totals);
		assert(ranks.size() == 3);
		assert(ranks[0].first == "10004" && ranks[0].second == 3);
		assert(ranks[1].first == "10002" && ranks[1].second == 2);
		assert(ranks[2].first == "10003" && ranks[2].second == 1);
	}

	void testReadinessValidation()
	{
		std::vector<TotalValidation::StudentReadiness> students;
		TotalValidation::StudentReadiness ok;
		ok.account = "10002";
		ok.name = "学生1";
		ok.moralFinished = true;
		ok.hasTotalRecord = true;
		ok.hasCourse = true;
		ok.hasTeacherMoral = true;
		ok.studentMoralCount = 2;
		students.push_back(ok);

		TotalValidation::StudentReadiness bad;
		bad.account = "10003";
		bad.name = "学生2";
		bad.moralFinished = false;
		bad.hasTotalRecord = false;
		bad.hasCourse = false;
		bad.hasTeacherMoral = false;
		bad.studentMoralCount = 1;
		students.push_back(bad);

		std::vector<std::string> unreviewed;
		unreviewed.push_back("10003");
		std::vector<std::string> errors = TotalValidation::validateReadiness(students, 2, unreviewed);
		assert(errors.size() == 6);
		assert(errors[0] == "学生2尚未完成思想品德打分");
		assert(errors[1] == "学生2缺少综测总表记录");
		assert(errors[2] == "学生2缺少课程成绩");
		assert(errors[3] == "学生2缺少班主任思想品德评分");
		assert(errors[4] == "学生2学生互评记录不完整");
		assert(errors[5] == "10003存在未审核的课外活动");
	}

	void testMoralItemText()
	{
		const std::vector<std::string>& studentItems = ConsoleInput::studentMoralItems();
		assert(studentItems.size() == 9);
		assert(studentItems[0] == "严格遵守校规校纪及学院各项规章制度");
		assert(studentItems[8] == "尊敬师长，关心集体，文明礼貌，团结同学，乐于助人");

		const std::vector<std::string>& teacherItems = ConsoleInput::teacherMoralItems();
		assert(teacherItems.size() == 3);
		assert(teacherItems[0] == "学生日常学习情况，上课出勤率，学习认真程度");
		assert(teacherItems[2] == "学生参与班会及班级集体活动情况");
	}

	void testScoreTableOutput()
	{
		StudentScore score;
		score.account = "10002";
		score.study = 100.0f;
		score.gpa = 4.0f;
		score.activity = 35.0f;
		score.moral = 100.0f;
		score.addition = 5.0f;
		score.total = 95.25f;
		score.rank = 1;

		std::ostringstream output;
		std::streambuf* oldBuffer = std::cout.rdbuf(output.rdbuf());
		ConsoleView::scores(std::vector<StudentScore>(1, score));
		std::cout.rdbuf(oldBuffer);

		const std::string text = output.str();
		assert(text.find("学号      学习成绩") != std::string::npos);
		assert(text.find("课外活动成绩    思想品德成绩") != std::string::npos);
		assert(text.find("10002     100") != std::string::npos);
		assert(text.find("95.25       1") != std::string::npos);
	}

	void testPasswordHasher()
	{
		const std::string stored = PasswordHasher::hash("888888");
		assert(PasswordHasher::isHash(stored));
		assert(PasswordHasher::verify("888888", stored));
		assert(!PasswordHasher::verify("wrong", stored));
		assert(!PasswordHasher::verify("888888", "888888"));
	}

	void writeFile(const std::filesystem::path& path, const std::string& content)
	{
		std::ofstream output(path);
		assert(output);
		output << content;
	}

	std::string readFile(const std::filesystem::path& path)
	{
		std::ifstream input(path, std::ios::binary);
		assert(input);
		std::ostringstream output;
		output << input.rdbuf();
		return output.str();
	}

	std::filesystem::path cleanTestDirectory(const std::string& name)
	{
		const std::filesystem::path directory = std::filesystem::path("x64") / "Debug" / name;
		std::filesystem::remove_all(directory);
		std::filesystem::create_directories(directory);
		return directory;
	}

	std::filesystem::path prepareRepositoryData()
	{
		const std::filesystem::path directory = cleanTestDirectory("test-data");
		writeFile(directory / "User.txt",
			"3\n"
			"10000\t测评小组\t" + PasswordHasher::hash("888888") + "\t2\t0\n"
			"10001\t辅导员\t" + PasswordHasher::hash("888888") + "\t1\t1\n"
			"10002\t学生1\t" + PasswordHasher::hash("888888") + "\t0\t1\n");
		writeFile(directory / "Course.txt",
			"1\n"
			"10002\t数学\t2\t90\n");
		writeFile(directory / "Act.txt",
			"2\n"
			"10002\t活动1\t课外活动\t10\t审核通过\n"
			"10002\t活动2\t课外活动\t20\t审核未通过\n");
		writeFile(directory / "Add.txt",
			"2\n"
			"10002\t竞赛\t附加分\t3\t审核通过\n"
			"10002\t证书\t附加分\t4\t审核通过\n");
		writeFile(directory / "Moral_s.txt",
			"1\n"
			"10002\t10002\t10\t10\t10\t10\t10\t10\t10\t10\t10\n");
		writeFile(directory / "Moral_t.txt",
			"1\n"
			"10002\t10001\t10\t10\t10\n");
		writeFile(directory / "Total.txt",
			"1\n"
			"10002\t0\t0\t0\t0\t0\t0\t0\n");
		return directory;
	}

	void testRepositoryAuthAndScoreService()
	{
		const std::filesystem::path directory = prepareRepositoryData();
		AssessmentRepository repository(directory.string());
		repository.loadAll();

		assert(repository.users().size() == 3);
		assert(repository.courses().count("10002") == 1);
		assert(repository.activities().count("10002") == 2);
		assert(repository.additions().count("10002") == 2);

		AuthService auth(repository);
		UserRecord user = auth.login("10002", "888888");
		assert(user.name == "学生1");
		bool loginFailed = false;
		try
		{
			auth.login("10002", "wrong");
		}
		catch (const std::runtime_error&)
		{
			loginFailed = true;
		}
		assert(loginFailed);
		auth.changePassword("10002", "888888", "999999");
		assert(PasswordHasher::isHash(repository.users().find("10002")->second.password));
		assert(auth.login("10002", "999999").account == "10002");

		ScoreService scoreService(repository);
		TotalBuildResult readiness = scoreService.validateBeforeBuild();
		assert(readiness.ready);
		scoreService.buildTotal();

		const StudentScore& score = repository.totals().find("10002")->second;
		assert(closeTo(score.gpa, 4.0f));
		assert(closeTo(score.study, 100.0f));
		assert(closeTo(score.activity, 10.0f));
		assert(closeTo(score.addition, 5.0f));
		assert(closeTo(score.total, 91.5f));
		assert(score.rank == 1);
	}

	void testDefaultStateAndTotalQueryGate()
	{
		const std::filesystem::path directory = cleanTestDirectory("test-default-state");
		writeFile(directory / "User.txt",
			"3\n"
			"10000\t测评小组\t" + PasswordHasher::hash("888888") + "\t2\t0\n"
			"10001\t辅导员\t" + PasswordHasher::hash("888888") + "\t1\t0\n"
			"10002\t学生1\t" + PasswordHasher::hash("888888") + "\t0\t0\n");
		writeFile(directory / "Course.txt", "0\n");
		writeFile(directory / "Act.txt", "0\n");
		writeFile(directory / "Add.txt", "0\n");
		writeFile(directory / "Moral_s.txt", "0\n");
		writeFile(directory / "Moral_t.txt", "0\n");
		writeFile(directory / "Total.txt", "0\n");

		AssessmentRepository repository(directory.string());
		repository.loadAll();

		assert(!repository.users().find("10002")->second.finishedMoralOrGeneratedTotal);
		QueryService query(repository);
		assert(!query.totalGenerated());
		assert(repository.totals().count("10002") == 1);

		MoralRecord record;
		record.receiverAccount = "10002";
		for (int index = 0; index < 9; ++index)
			record.scores.push_back(10.0f);
		MoralService(repository).submitStudentMoral("10002", std::vector<MoralRecord>(1, record));
		assert(repository.users().find("10002")->second.finishedMoralOrGeneratedTotal);

		repository.users().find("10000")->second.finishedMoralOrGeneratedTotal = true;
		assert(query.totalGenerated());
	}

	void testMissingRuntimeFiles()
	{
		const std::filesystem::path directory = cleanTestDirectory("test-missing-runtime-files");
		writeFile(directory / "User.txt",
			"3\n"
			"10000\t测评小组\t" + PasswordHasher::hash("888888") + "\t2\t0\n"
			"10001\t辅导员\t" + PasswordHasher::hash("888888") + "\t1\t0\n"
			"10002\t学生1\t" + PasswordHasher::hash("888888") + "\t0\t0\n");

		AssessmentRepository repository(directory.string());
		repository.loadAll();

		assert(repository.users().size() == 3);
		assert(repository.courses().empty());
		assert(repository.activities().empty());
		assert(repository.additions().empty());
		assert(repository.studentMoralsByReceiver().empty());
		assert(repository.teacherMorals().empty());
		assert(repository.totals().count("10002") == 1);
	}

	void testMissingUserFileStillFails()
	{
		const std::filesystem::path directory = cleanTestDirectory("test-missing-user-file");
		AssessmentRepository repository(directory.string());
		bool thrown = false;
		try
		{
			repository.loadAll();
		}
		catch (const std::runtime_error&)
		{
			thrown = true;
		}
		assert(thrown);
	}

	void testMalformedRuntimeFileStillFails()
	{
		const std::filesystem::path directory = cleanTestDirectory("test-malformed-runtime-file");
		writeFile(directory / "User.txt",
			"1\n"
			"10002\t学生1\t" + PasswordHasher::hash("888888") + "\t0\t0\n");
		writeFile(directory / "Course.txt", "bad\n");

		AssessmentRepository repository(directory.string());
		bool thrown = false;
		try
		{
			repository.loadAll();
		}
		catch (const std::runtime_error&)
		{
			thrown = true;
		}
		assert(thrown);
	}

	void testMutationServices()
	{
		const std::filesystem::path directory = prepareRepositoryData();
		AssessmentRepository repository(directory.string());
		repository.loadAll();

		CourseService courseService(repository);
		courseService.createCourse("10002", "英语", 1.0f, 85.0f);
		assert(repository.courses().count("10002") == 2);
		const std::string protectedCourseFile = readFile(directory / "Course.txt");
		assert(protectedCourseFile.find("SQAS-DPAPI-1\n") == 0);
		assert(protectedCourseFile.find("英语") == std::string::npos);
		AssessmentRepository protectedRepository(directory.string());
		protectedRepository.loadAll();
		assert(protectedRepository.courses().count("10002") == 2);

		CourseRecord course;
		course.account = "10002";
		course.name = "物理";
		course.credit = 2.0f;
		course.grade = 82.0f;
		courseService.updateCourse(1, course);
		assert(courseService.listAll()[0].name == "物理");
		courseService.deleteCourse(1);
		assert(repository.courses().count("10002") == 1);

		ActivityService activityService(repository);
		activityService.createActivity("10002", "新活动", 5.0f);
		assert(activityService.listForStudent("10002").size() == 3);
		activityService.approveActivity(3);
		assert(activityService.listAll()[2].status == ReviewStatus::Approved);
		bool approvedUpdateFailed = false;
		try
		{
			activityService.updateStudentActivity("10002", 3, "改名", 6.0f);
		}
		catch (const std::runtime_error&)
		{
			approvedUpdateFailed = true;
		}
		assert(approvedUpdateFailed);

		AdditionService additionService(repository);
		additionService.createPersonalAddition("10002", "新附加", 1.0f);
		assert(additionService.listAll().size() == 3);
		additionService.deleteAddition(3);
		assert(additionService.listAll().size() == 2);

		MoralService moralService(repository);
		assert(moralService.studentMoralsGivenBy("10002").size() == 1);
		assert(moralService.teacherMorals().size() == 1);
		QueryService query(repository);
		assert(query.isStudent("10002"));
		assert(query.userName("10002") == "学生1");
	}
}

int main()
{
	testTextTable();
	testScoreCalculator();
	testRank();
	testReadinessValidation();
	testMoralItemText();
	testScoreTableOutput();
	testPasswordHasher();
	testMissingRuntimeFiles();
	testMissingUserFileStillFails();
	testMalformedRuntimeFileStillFails();
	testRepositoryAuthAndScoreService();
	testDefaultStateAndTotalQueryGate();
	testMutationServices();
	return 0;
}
