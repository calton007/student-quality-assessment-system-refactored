#include "AppConfig.h"
#include "ScoreCalculator.h"
#include "SecureFileStore.h"
#include "TextTable.h"
#include "TotalValidation.h"
#include "AssessmentRepository.h"
#include "ActivityService.h"
#include "AdditionService.h"
#include "AuthService.h"
#include "CourseService.h"
#include "ConsoleInput.h"
#include "ConsoleTable.h"
#include "ConsoleView.h"
#include "GroupConsole.h"
#include "MoralService.h"
#include "PasswordHasher.h"
#include "QueryService.h"
#include "ScoreService.h"
#include "StudentConsole.h"
#include "TeacherConsole.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

namespace
{
	bool closeTo(float left, float right)
	{
		return std::fabs(left - right) < 0.001f;
	}

	bool containsItem(const std::vector<std::string>& items, const std::string& value)
	{
		return std::find(items.begin(), items.end(), value) != items.end();
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
		assert(errors[4] == "学生2学生互评记录缺失");
		assert(errors[5] == "10003存在未审核的课外活动");

		students.clear();
		TotalValidation::StudentReadiness duplicate;
		duplicate.account = "10002";
		duplicate.name = "学生1";
		duplicate.moralFinished = true;
		duplicate.hasTotalRecord = true;
		duplicate.hasCourse = true;
		duplicate.hasTeacherMoral = true;
		duplicate.studentMoralCount = 3;
		students.push_back(duplicate);
		errors = TotalValidation::validateReadiness(students, 2, std::vector<std::string>());
		assert(errors.size() == 1);
		assert(errors[0] == "学生1学生互评记录重复");
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

	void testEmptyTableOutput()
	{
		std::ostringstream output;
		std::streambuf* oldBuffer = std::cout.rdbuf(output.rdbuf());
		ConsoleView::courses(std::vector<CourseRecord>());
		std::cout.rdbuf(oldBuffer);

		const std::string text = output.str();
		assert(text.find("课程名") != std::string::npos);
		assert(text.find("暂无数据") != std::string::npos);
	}

	void testChineseDisplayWidth()
	{
		assert(ConsoleTable::displayWidth("学号") == 4);
		assert(ConsoleTable::displayWidth("GPA") == 3);
		assert(ConsoleTable::displayWidth("学生A") == 5);
	}

	void testMenuInputRetries()
	{
		std::istringstream input("x\n9\n2\n");
		std::ostringstream output;
		std::streambuf* oldInput = std::cin.rdbuf(input.rdbuf());
		std::streambuf* oldOutput = std::cout.rdbuf(output.rdbuf());
		const char choice = ConsoleInput::menuChoice(3);
		std::cin.rdbuf(oldInput);
		std::cout.rdbuf(oldOutput);

		assert(choice == '2');
		assert(output.str().find("请输入正确的菜单编号") != std::string::npos);
	}

	void testScoreInputBoundaries()
	{
		std::istringstream input("abc\n-1\n101\n0\n100\n");
		std::ostringstream output;
		std::streambuf* oldInput = std::cin.rdbuf(input.rdbuf());
		std::streambuf* oldOutput = std::cout.rdbuf(output.rdbuf());
		const float minValue = ConsoleInput::score(0.0f, 100.0f, "成绩");
		const float maxValue = ConsoleInput::score(0.0f, 100.0f, "成绩");
		std::cin.rdbuf(oldInput);
		std::cout.rdbuf(oldOutput);

		assert(closeTo(minValue, 0.0f));
		assert(closeTo(maxValue, 100.0f));
		assert(output.str().find("请输入0-100之间的成绩") != std::string::npos);
	}

	void testConfirmInput()
	{
		std::istringstream input("x\n2\n1\n0\n");
		std::ostringstream output;
		std::streambuf* oldInput = std::cin.rdbuf(input.rdbuf());
		std::streambuf* oldOutput = std::cout.rdbuf(output.rdbuf());
		const bool confirmed = ConsoleInput::confirm("保存");
		const bool cancelled = ConsoleInput::confirm("保存");
		std::cin.rdbuf(oldInput);
		std::cout.rdbuf(oldOutput);

		assert(confirmed);
		assert(!cancelled);
		assert(output.str().find("请输入1确认或0取消") != std::string::npos);
	}

	void testGeneratedTotalHidesMutationMenuItems()
	{
		const std::vector<std::string> unlockedGroup = GroupConsole::homeMenuItems(false);
		assert(containsItem(unlockedGroup, "学习成绩管理"));
		assert(containsItem(unlockedGroup, "附加分管理"));
		assert(containsItem(unlockedGroup, "审核课外活动加分"));
		assert(containsItem(unlockedGroup, "查询项目"));

		const std::vector<std::string> lockedGroup = GroupConsole::homeMenuItems(true);
		assert(!containsItem(lockedGroup, "学习成绩管理"));
		assert(!containsItem(lockedGroup, "附加分管理"));
		assert(!containsItem(lockedGroup, "审核课外活动加分"));
		assert(containsItem(lockedGroup, "查询项目"));
		assert(containsItem(lockedGroup, "综测成绩生成"));
		assert(lockedGroup.size() == 5);

		const std::vector<std::string> unlockedStudent = StudentConsole::homeMenuItems(false);
		assert(containsItem(unlockedStudent, "思想品德项目"));
		assert(containsItem(unlockedStudent, "课外活动项目"));
		assert(containsItem(unlockedStudent, "查询"));

		const std::vector<std::string> lockedStudent = StudentConsole::homeMenuItems(true);
		assert(!containsItem(lockedStudent, "思想品德项目"));
		assert(!containsItem(lockedStudent, "课外活动项目"));
		assert(containsItem(lockedStudent, "查询"));
		assert(lockedStudent.size() == 4);

		const std::vector<std::string> unlockedTeacher = TeacherConsole::homeMenuItems(false);
		assert(containsItem(unlockedTeacher, "思想品德打分"));
		assert(containsItem(unlockedTeacher, "修改思想品德打分"));
		assert(containsItem(unlockedTeacher, "修改密码"));

		const std::vector<std::string> lockedTeacher = TeacherConsole::homeMenuItems(true);
		assert(!containsItem(lockedTeacher, "思想品德打分"));
		assert(!containsItem(lockedTeacher, "修改思想品德打分"));
		assert(containsItem(lockedTeacher, "修改密码"));
		assert(lockedTeacher.size() == 3);
	}

	void testTextRequiredReadsNonBlankInput()
	{
		std::istringstream input("课程\n");
		std::ostringstream output;
		std::streambuf* oldInput = std::cin.rdbuf(input.rdbuf());
		std::streambuf* oldOutput = std::cout.rdbuf(output.rdbuf());
		const std::string value = ConsoleInput::textRequired("课程名:");
		std::cin.rdbuf(oldInput);
		std::cout.rdbuf(oldOutput);

		assert(value == "课程");
		assert(output.str().find("课程名:") != std::string::npos);
	}

	void testOptionalTextCanCancel()
	{
		std::istringstream input("0\n活动\n");
		std::ostringstream output;
		std::streambuf* oldInput = std::cin.rdbuf(input.rdbuf());
		std::streambuf* oldOutput = std::cout.rdbuf(output.rdbuf());
		const std::string cancelled = ConsoleInput::optionalText("活动名称:");
		const std::string value = ConsoleInput::optionalText("活动名称:");
		std::cin.rdbuf(oldInput);
		std::cout.rdbuf(oldOutput);

		assert(cancelled.empty());
		assert(value == "活动");
	}

	void testChoiceRetriesInvalidInput()
	{
		std::istringstream input("abc\n3\n2\n");
		std::ostringstream output;
		std::streambuf* oldInput = std::cin.rdbuf(input.rdbuf());
		std::streambuf* oldOutput = std::cout.rdbuf(output.rdbuf());
		const int value = ConsoleInput::choice("1:批量 2:单个", 2);
		std::cin.rdbuf(oldInput);
		std::cout.rdbuf(oldOutput);

		assert(value == 2);
		assert(output.str().find("请输入正确的菜单编号") != std::string::npos);
	}

	void testMenuShowsLocationAndUser()
	{
		UserRecord user;
		user.account = "10002";
		user.name = "学生1";

		std::ostringstream output;
		std::streambuf* oldBuffer = std::cout.rdbuf(output.rdbuf());
		ConsoleView::menu("学生首页 / 查询", user, std::vector<std::string>(1, "返回"));
		std::cout.rdbuf(oldBuffer);

		const std::string text = output.str();
		assert(text.find("当前位置: 学生首页 / 查询") != std::string::npos);
		assert(text.find("当前用户: 学生1(10002)") != std::string::npos);
		assert(text.find("[1] 返回") != std::string::npos);
	}

	void testOperationText()
	{
		std::ostringstream output;
		std::streambuf* oldBuffer = std::cout.rdbuf(output.rdbuf());
		ConsoleView::operation("学生1", "10002", "学习成绩", "录入");
		std::cout.rdbuf(oldBuffer);

		assert(output.str().find("正在操作: 学生1(10002) / 学习成绩 / 录入") != std::string::npos);
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

	void setLocalAppData(const std::string& value)
	{
		const int result = _putenv_s("LOCALAPPDATA", value.c_str());
		assert(result == 0);
	}

	void testAppConfig()
	{
		char* existing = NULL;
		size_t existingLength = 0;
		const bool hadOriginal = _dupenv_s(&existing, &existingLength, "LOCALAPPDATA") == 0 && existing != NULL;
		const std::string original = hadOriginal ? existing : "";
		std::free(existing);

		const std::filesystem::path localAppData = cleanTestDirectory("test-app-config-local");
		setLocalAppData(localAppData.string());

		char appName[] = "app.exe";
		char* defaultArgs[] = { appName };
		AppConfig defaultConfig = AppConfig::parse(1, defaultArgs);
		assert(defaultConfig.userFilePath() == "User.txt");
		assert(defaultConfig.runtimeDataDirectory() == (localAppData / "StudentQualityAssessment").string());
		assert(std::filesystem::is_directory(defaultConfig.runtimeDataDirectory()));

		const std::filesystem::path customDataDirectory = cleanTestDirectory("test-app-config-custom") / "nested";
		char option[] = "--data-dir";
		std::string customText = customDataDirectory.string();
		char* customPath = const_cast<char*>(customText.c_str());
		char* customArgs[] = { appName, option, customPath };
		AppConfig customConfig = AppConfig::parse(3, customArgs);
		assert(customConfig.runtimeDataDirectory() == customDataDirectory.string());
		assert(std::filesystem::is_directory(customConfig.runtimeDataDirectory()));

		bool missingValueThrown = false;
		try
		{
			char* badArgs[] = { appName, option };
			AppConfig::parse(2, badArgs);
		}
		catch (const std::runtime_error&)
		{
			missingValueThrown = true;
		}
		assert(missingValueThrown);

		bool unknownThrown = false;
		try
		{
			char unknown[] = "--unknown";
			char* badArgs[] = { appName, unknown };
			AppConfig::parse(2, badArgs);
		}
		catch (const std::runtime_error&)
		{
			unknownThrown = true;
		}
		assert(unknownThrown);

		setLocalAppData("");
		bool missingEnvThrown = false;
		try
		{
			AppConfig::parse(1, defaultArgs);
		}
		catch (const std::runtime_error&)
		{
			missingEnvThrown = true;
		}
		assert(missingEnvThrown);

		setLocalAppData(original);
	}

	void testRepositorySeparatesUserFileAndRuntimeData()
	{
		const std::filesystem::path directory = cleanTestDirectory("test-repository-paths");
		const std::filesystem::path userFile = directory / "User.txt";
		const std::filesystem::path runtimeDirectory = directory / "runtime";
		std::filesystem::create_directories(runtimeDirectory);
		writeFile(userFile,
			"1\n"
			"10002\t学生1\t" + PasswordHasher::hash("888888") + "\t0\n");
		writeFile(runtimeDirectory / "Course.txt",
			"1\n"
			"10002\t数学\t2\t90\n");

		AssessmentRepository repository(userFile.string(), runtimeDirectory.string(), NULL);
		repository.loadAll();
		assert(repository.users().size() == 1);
		assert(repository.courses().count("10002") == 1);

		repository.saveCourses();
		assert(!std::filesystem::exists(directory / "Course.txt"));
		assert(std::filesystem::exists(runtimeDirectory / "Course.txt"));
	}

	void testBackupFileGenerated()
	{
		const std::filesystem::path directory = cleanTestDirectory("test-backup-file");
		const std::filesystem::path file = directory / "data.txt";
		writeFile(file, "old");

		SecureFileStore::writePlainText(file.string(), "new");

		assert(readFile(file) == "new");
		assert(readFile(file.string() + ".bak") == "old");
	}

	void testFailedSaveKeepsOldFile()
	{
		const std::filesystem::path directory = cleanTestDirectory("test-failed-save-keeps-old");
		const std::filesystem::path file = directory / "data.txt";
		writeFile(file, "old");
		std::filesystem::create_directory(file.string() + ".tmp");

		bool thrown = false;
		try
		{
			SecureFileStore::writePlainText(file.string(), "new");
		}
		catch (const std::runtime_error&)
		{
			thrown = true;
		}

		assert(thrown);
		assert(readFile(file) == "old");
	}

	std::filesystem::path prepareRepositoryData()
	{
		const std::filesystem::path directory = cleanTestDirectory("test-data");
		writeFile(directory / "User.txt",
			"3\n"
			"10000\t测评小组\t" + PasswordHasher::hash("888888") + "\t2\n"
			"10001\t辅导员\t" + PasswordHasher::hash("888888") + "\t1\n"
			"10002\t学生1\t" + PasswordHasher::hash("888888") + "\t0\n");
		writeFile(directory / "AssessmentStatus.txt",
			"SQAS-STATUS-1\n"
			"total_generated 0\n"
			"student_moral_finished 1\n"
			"10002\n"
			"teacher_moral_finished 1\n"
			"10001\n");
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
		assert(readiness.students.size() == 1);
		assert(readiness.students[0].account == "10002");
		assert(readiness.pendingActivityAccounts.empty());
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
			"10000\t测评小组\t" + PasswordHasher::hash("888888") + "\t2\n"
			"10001\t辅导员\t" + PasswordHasher::hash("888888") + "\t1\n"
			"10002\t学生1\t" + PasswordHasher::hash("888888") + "\t0\n");
		writeFile(directory / "Course.txt", "0\n");
		writeFile(directory / "Act.txt", "0\n");
		writeFile(directory / "Add.txt", "0\n");
		writeFile(directory / "Moral_s.txt", "0\n");
		writeFile(directory / "Moral_t.txt", "0\n");
		writeFile(directory / "Total.txt", "0\n");

		AssessmentRepository repository(directory.string());
		repository.loadAll();

		assert(repository.status().studentMoralFinishedAccounts.empty());
		TotalBuildResult readiness = ScoreService(repository).validateBeforeBuild();
		assert(!readiness.ready);
		assert(readiness.students.size() == 1);
		assert(readiness.students[0].account == "10002");
		assert(!readiness.students[0].moralFinished);
		assert(!readiness.students[0].hasCourse);
		assert(!readiness.students[0].hasTeacherMoral);
		assert(readiness.students[0].studentMoralCount == 0);
		QueryService query(repository);
		assert(!query.totalGenerated());
		assert(repository.totals().count("10002") == 1);

		MoralRecord record;
		record.receiverAccount = "10002";
		for (int index = 0; index < 9; ++index)
			record.scores.push_back(10.0f);
		MoralService(repository).submitStudentMoral("10002", std::vector<MoralRecord>(1, record));
		assert(repository.status().studentMoralFinishedAccounts.count("10002") == 1);

		repository.status().totalGenerated = true;
		assert(query.totalGenerated());
	}

	void testMissingRuntimeFiles()
	{
		const std::filesystem::path directory = cleanTestDirectory("test-missing-runtime-files");
		writeFile(directory / "User.txt",
			"3\n"
			"10000\t测评小组\t" + PasswordHasher::hash("888888") + "\t2\n"
			"10001\t辅导员\t" + PasswordHasher::hash("888888") + "\t1\n"
			"10002\t学生1\t" + PasswordHasher::hash("888888") + "\t0\n");

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

	void testLegacyUserStatusMigration()
	{
		const std::filesystem::path directory = cleanTestDirectory("test-legacy-user-status-migration");
		writeFile(directory / "User.txt",
			"3\n"
			"10000\t测评小组\t" + PasswordHasher::hash("888888") + "\t2\t1\n"
			"10001\t辅导员\t" + PasswordHasher::hash("888888") + "\t1\t1\n"
			"10002\t学生1\t" + PasswordHasher::hash("888888") + "\t0\t1\n");
		writeFile(directory / "Moral_s.txt",
			"1\n"
			"10002\t10002\t10\t10\t10\t10\t10\t10\t10\t10\t10\n");
		writeFile(directory / "Moral_t.txt",
			"1\n"
			"10002\t10001\t10\t10\t10\n");
		writeFile(directory / "Total.txt",
			"1\n"
			"10002\t100\t4\t10\t100\t5\t91.5\t1\n");

		AssessmentRepository repository(directory.string());
		repository.loadAll();

		assert(repository.status().totalGenerated);
		assert(repository.status().studentMoralFinishedAccounts.count("10002") == 1);
		assert(repository.status().teacherMoralFinishedAccounts.count("10001") == 1);
		assert(readFile(directory / "User.txt").find("\t0\t1") == std::string::npos);
		const std::string statusFile = readFile(directory / "AssessmentStatus.txt");
		assert(statusFile.find("SQAS-DPAPI-1\n") == 0);
		assert(statusFile.find("student_moral_finished") == std::string::npos);
	}

	void testMalformedStatusFileFails()
	{
		const std::filesystem::path directory = cleanTestDirectory("test-malformed-status-file");
		writeFile(directory / "User.txt",
			"1\n"
			"10002\t学生1\t" + PasswordHasher::hash("888888") + "\t0\n");
		writeFile(directory / "AssessmentStatus.txt", "bad\n");

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

	void testDuplicateStudentMoralFileFails()
	{
		const std::filesystem::path directory = cleanTestDirectory("test-duplicate-student-moral-file");
		writeFile(directory / "User.txt",
			"2\n"
			"10002\t学生1\t" + PasswordHasher::hash("888888") + "\t0\n"
			"10003\t学生2\t" + PasswordHasher::hash("888888") + "\t0\n");
		writeFile(directory / "Moral_s.txt",
			"2\n"
			"10002\t10003\t10\t10\t10\t10\t10\t10\t10\t10\t10\n"
			"10002\t10003\t9\t9\t9\t9\t9\t9\t9\t9\t9\n");

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
			"10002\t学生1\t" + PasswordHasher::hash("888888") + "\t0\n");
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
		bool duplicateMoralFailed = false;
		try
		{
			MoralRecord record;
			record.receiverAccount = "10002";
			for (int index = 0; index < 9; ++index)
				record.scores.push_back(10.0f);
			moralService.submitStudentMoral("10002", std::vector<MoralRecord>(1, record));
		}
		catch (const std::runtime_error&)
		{
			duplicateMoralFailed = true;
		}
		assert(duplicateMoralFailed);
		QueryService query(repository);
		assert(query.isStudent("10002"));
		assert(query.userName("10002") == "学生1");
	}

	void testTotalGenerationLocksMutationsAndRevokeUnlocks()
	{
		const std::filesystem::path directory = prepareRepositoryData();
		AssessmentRepository repository(directory.string());
		repository.loadAll();
		ScoreService scoreService(repository);
		scoreService.buildTotal();
		assert(repository.status().totalGenerated);

		bool courseThrown = false;
		try { CourseService(repository).createCourse("10002", "锁定", 1.0f, 80.0f); }
		catch (const std::runtime_error&) { courseThrown = true; }
		assert(courseThrown);

		bool activityThrown = false;
		try { ActivityService(repository).approveActivity(1); }
		catch (const std::runtime_error&) { activityThrown = true; }
		assert(activityThrown);

		bool additionThrown = false;
		try { AdditionService(repository).createPersonalAddition("10002", "锁定", 1.0f); }
		catch (const std::runtime_error&) { additionThrown = true; }
		assert(additionThrown);

		bool moralThrown = false;
		try
		{
			MoralRecord record = MoralService(repository).studentMoralsGivenBy("10002")[0];
			MoralService(repository).updateStudentMoral("10002", 1, record);
		}
		catch (const std::runtime_error&) { moralThrown = true; }
		assert(moralThrown);

		scoreService.revokeTotal();
		assert(!repository.status().totalGenerated);
		const StudentScore& score = repository.totals().find("10002")->second;
		assert(closeTo(score.study, 0.0f));
		assert(closeTo(score.gpa, 0.0f));
		assert(closeTo(score.activity, 0.0f));
		assert(closeTo(score.moral, 0.0f));
		assert(closeTo(score.addition, 0.0f));
		assert(closeTo(score.total, 0.0f));
		assert(score.rank == 0);

		CourseService(repository).createCourse("10002", "解锁", 1.0f, 80.0f);
		assert(repository.courses().count("10002") == 2);
	}

void testCancelledModeDoesNotSave()
{
	const std::filesystem::path directory = prepareRepositoryData();
	AssessmentRepository repository(directory.string());
	repository.loadAll();
		const size_t originalCount = repository.courses().size();

		std::istringstream input("0\n");
		std::ostringstream output;
		std::streambuf* oldInput = std::cin.rdbuf(input.rdbuf());
		std::streambuf* oldOutput = std::cout.rdbuf(output.rdbuf());
		const int mode = ConsoleInput::choice("1:批量录入成绩 2:单个录入成绩", 2);
		std::cin.rdbuf(oldInput);
		std::cout.rdbuf(oldOutput);

		if (mode != 0)
			CourseService(repository).createCourse("10002", "不应保存", 1.0f, 80.0f);
	assert(repository.courses().size() == originalCount);
}

void testDefaultAccountFullFlow()
{
	const std::filesystem::path directory = cleanTestDirectory("test-default-account-full-flow");
	writeFile(directory / "User.txt",
		"3\n"
		"10000\t测评小组\t" + PasswordHasher::hash("888888") + "\t2\n"
		"10001\t辅导员\t" + PasswordHasher::hash("888888") + "\t1\n"
		"10002\t学生1\t" + PasswordHasher::hash("888888") + "\t0\n");

	AssessmentRepository repository(directory.string());
	repository.loadAll();

	AuthService auth(repository);
	auth.login("10000", "888888");
	auth.login("10001", "888888");
	auth.login("10002", "888888");

	CourseService courseService(repository);
	courseService.createCourse("10002", "数学", 2.0f, 90.0f);

	MoralService moralService(repository);
	MoralRecord studentRecord;
	studentRecord.receiverAccount = "10002";
	studentRecord.giverAccount = "10002";
	for (size_t index = 0; index < 9; ++index)
		studentRecord.scores.push_back(10.0f);
	moralService.submitStudentMoral("10002", std::vector<MoralRecord>(1, studentRecord));

	MoralRecord teacherRecord;
	teacherRecord.receiverAccount = "10002";
	for (size_t index = 0; index < 3; ++index)
		teacherRecord.scores.push_back(10.0f);
	moralService.submitTeacherMoral("10001", std::vector<MoralRecord>(1, teacherRecord));

	ScoreService scoreService(repository);
	const TotalBuildResult readiness = scoreService.validateBeforeBuild();
	assert(readiness.ready);
	scoreService.buildTotal();
	assert(repository.status().totalGenerated);
	assert(repository.totals().find("10002")->second.total > 0.0f);

	QueryService query(repository);
	const StudentScore& score = query.scoreFor("10002");
	assert(closeTo(score.study, 100.0f));

	scoreService.revokeTotal();
	assert(!repository.status().totalGenerated);
}
}

int main()
{
#ifdef _MSC_VER
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
#endif
	testAppConfig();
	testTextTable();
	testScoreCalculator();
	testRank();
	testReadinessValidation();
	testMoralItemText();
	testScoreTableOutput();
	testEmptyTableOutput();
	testChineseDisplayWidth();
	testMenuInputRetries();
	testScoreInputBoundaries();
	testConfirmInput();
	testGeneratedTotalHidesMutationMenuItems();
	testTextRequiredReadsNonBlankInput();
	testOptionalTextCanCancel();
	testChoiceRetriesInvalidInput();
	testMenuShowsLocationAndUser();
	testOperationText();
	testPasswordHasher();
	testMissingRuntimeFiles();
	testLegacyUserStatusMigration();
	testMissingUserFileStillFails();
	testMalformedRuntimeFileStillFails();
	testMalformedStatusFileFails();
	testDuplicateStudentMoralFileFails();
	testRepositorySeparatesUserFileAndRuntimeData();
	testBackupFileGenerated();
	testFailedSaveKeepsOldFile();
	testRepositoryAuthAndScoreService();
	testDefaultStateAndTotalQueryGate();
	testMutationServices();
	testTotalGenerationLocksMutationsAndRevokeUnlocks();
	testCancelledModeDoesNotSave();
	testDefaultAccountFullFlow();
	return 0;
}
