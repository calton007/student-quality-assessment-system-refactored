#include "ScoreService.h"

#include "ScoreCalculator.h"
#include "TotalValidation.h"

#include <map>
#include <stdexcept>

ScoreService::ScoreService(AssessmentRepository& repository)
	: repository_(repository)
{
}

TotalBuildResult ScoreService::validateBeforeBuild() const
{
	int studentCount = 0;
	std::vector<TotalValidation::StudentReadiness> students;
	for (std::map<std::string, UserRecord>::const_iterator iter = repository_.users().begin(); iter != repository_.users().end(); ++iter)
	{
		if (iter->second.role == UserRole::Student)
			++studentCount;
	}

	for (std::map<std::string, UserRecord>::const_iterator iter = repository_.users().begin(); iter != repository_.users().end(); ++iter)
	{
		if (iter->second.role != UserRole::Student)
			continue;
		TotalValidation::StudentReadiness readiness;
		readiness.account = iter->first;
		readiness.name = iter->second.name;
		readiness.moralFinished = repository_.status().studentMoralFinishedAccounts.find(iter->first) != repository_.status().studentMoralFinishedAccounts.end();
		readiness.hasTotalRecord = repository_.totals().find(iter->first) != repository_.totals().end();
		readiness.hasCourse = repository_.courses().find(iter->first) != repository_.courses().end();
		readiness.hasTeacherMoral = repository_.teacherMorals().find(iter->first) != repository_.teacherMorals().end();
		readiness.studentMoralCount = static_cast<int>(repository_.studentMoralsByReceiver().count(iter->first));
		students.push_back(readiness);
	}

	std::vector<std::string> pendingActivities;
	for (std::multimap<std::string, ActivityRecord>::const_iterator iter = repository_.activities().begin(); iter != repository_.activities().end(); ++iter)
	{
		if (iter->second.status == ReviewStatus::Pending)
			pendingActivities.push_back(iter->second.account);
	}

	TotalBuildResult result;
	result.errors = TotalValidation::validateReadiness(students, studentCount, pendingActivities);
	result.students = students;
	result.pendingActivityAccounts = pendingActivities;
	result.ready = result.errors.empty();
	return result;
}

void ScoreService::buildTotal()
{
	if (repository_.status().totalGenerated)
		throw std::runtime_error("综测成绩已生成，请先撤销后再生成");
	TotalBuildResult readiness = validateBeforeBuild();
	if (!readiness.ready)
		throw std::runtime_error("cannot build total before required data is complete");

	ensureTotalRecords();
	buildMoral();
	buildActivity();
	buildAddition();
	buildGpaAndStudy();

	std::vector<std::pair<std::string, float>> totals;
	for (std::map<std::string, StudentScore>::iterator iter = repository_.totals().begin(); iter != repository_.totals().end(); ++iter)
	{
		iter->second.total = ScoreCalculator::totalScore(iter->second.study, iter->second.activity, iter->second.moral, iter->second.addition);
		totals.push_back(std::pair<std::string, float>(iter->first, iter->second.total));
	}
	std::vector<std::pair<std::string, int>> ranks = ScoreCalculator::rankByTotal(totals);
	for (std::vector<std::pair<std::string, int>>::const_iterator iter = ranks.begin(); iter != ranks.end(); ++iter)
		repository_.totals()[iter->first].rank = iter->second;

	repository_.saveTotals();
	repository_.status().totalGenerated = true;
	repository_.saveStatus();
}

void ScoreService::revokeTotal()
{
	if (!repository_.status().totalGenerated)
		throw std::runtime_error("尚未生成综测成绩，无需撤销");
	clearTotals();
	repository_.saveTotals();
	repository_.status().totalGenerated = false;
	repository_.saveStatus();
}

void ScoreService::ensureTotalRecords()
{
	for (std::map<std::string, UserRecord>::const_iterator iter = repository_.users().begin(); iter != repository_.users().end(); ++iter)
	{
		if (iter->second.role == UserRole::Student && repository_.totals().find(iter->first) == repository_.totals().end())
		{
			StudentScore score;
			score.account = iter->first;
			repository_.totals().insert(std::pair<std::string, StudentScore>(iter->first, score));
		}
	}
}

void ScoreService::buildMoral()
{
	std::multimap<float, std::string> ordered;
	for (std::map<std::string, UserRecord>::const_iterator user = repository_.users().begin(); user != repository_.users().end(); ++user)
	{
		if (user->second.role != UserRole::Student)
			continue;
		float sum = 0.0f;
		int divisor = 0;
		std::pair<std::multimap<std::string, MoralRecord>::const_iterator, std::multimap<std::string, MoralRecord>::const_iterator> range =
			repository_.studentMoralsByReceiver().equal_range(user->first);
		for (; range.first != range.second; ++range.first)
		{
			sum += sumScores(range.first->second);
			divisor += 10;
		}
		sum += sumScores(repository_.teacherMorals().find(user->first)->second);
		divisor += 3;
		ordered.insert(std::pair<float, std::string>(sum / divisor, user->first));
	}

	float score = 100.0f;
	for (std::multimap<float, std::string>::reverse_iterator iter = ordered.rbegin(); iter != ordered.rend(); ++iter, --score)
		repository_.totals()[iter->second].moral = score;
}

void ScoreService::buildActivity()
{
	for (std::map<std::string, UserRecord>::const_iterator user = repository_.users().begin(); user != repository_.users().end(); ++user)
	{
		if (user->second.role != UserRole::Student)
			continue;
		float sum = 0.0f;
		std::pair<std::multimap<std::string, ActivityRecord>::const_iterator, std::multimap<std::string, ActivityRecord>::const_iterator> range =
			repository_.activities().equal_range(user->first);
		for (; range.first != range.second; ++range.first)
		{
			if (range.first->second.status == ReviewStatus::Approved)
				sum += range.first->second.grade;
		}
		repository_.totals()[user->first].activity = sum;
	}
}

void ScoreService::buildAddition()
{
	for (std::map<std::string, UserRecord>::const_iterator user = repository_.users().begin(); user != repository_.users().end(); ++user)
	{
		if (user->second.role != UserRole::Student)
			continue;
		float sum = 0.0f;
		std::pair<std::multimap<std::string, ActivityRecord>::const_iterator, std::multimap<std::string, ActivityRecord>::const_iterator> range =
			repository_.additions().equal_range(user->first);
		for (; range.first != range.second; ++range.first)
			sum += range.first->second.grade;
		repository_.totals()[user->first].addition = ScoreCalculator::cappedAddition(sum);
	}
}

void ScoreService::buildGpaAndStudy()
{
	for (std::map<std::string, UserRecord>::const_iterator user = repository_.users().begin(); user != repository_.users().end(); ++user)
	{
		if (user->second.role != UserRole::Student)
			continue;
		std::vector<std::pair<float, float>> courses;
		std::pair<std::multimap<std::string, CourseRecord>::const_iterator, std::multimap<std::string, CourseRecord>::const_iterator> range =
			repository_.courses().equal_range(user->first);
		for (; range.first != range.second; ++range.first)
			courses.push_back(std::pair<float, float>(range.first->second.credit, range.first->second.grade));
		StudentScore& score = repository_.totals()[user->first];
		score.gpa = ScoreCalculator::gpa(courses);
		score.study = ScoreCalculator::studyScore(score.gpa);
	}
}

void ScoreService::clearTotals()
{
	ensureTotalRecords();
	for (std::map<std::string, StudentScore>::iterator iter = repository_.totals().begin(); iter != repository_.totals().end(); ++iter)
	{
		iter->second.study = 0.0f;
		iter->second.gpa = 0.0f;
		iter->second.activity = 0.0f;
		iter->second.moral = 0.0f;
		iter->second.addition = 0.0f;
		iter->second.total = 0.0f;
		iter->second.rank = 0;
	}
}
