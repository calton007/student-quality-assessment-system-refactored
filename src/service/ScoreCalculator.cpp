#include "ScoreCalculator.h"

#include <map>

namespace
{
	float floorToTwoDecimals(float value)
	{
		value *= 100;
		value = static_cast<int>(value);
		value /= 100;
		return value;
	}
}

namespace ScoreCalculator
{
	float gradePoint(float grade)
	{
		if (grade >= 90 && grade <= 100)
			return 4.0f;
		if (grade >= 85 && grade < 90)
			return 3.7f;
		if (grade >= 82 && grade < 85)
			return 3.3f;
		if (grade >= 78 && grade < 82)
			return 3.0f;
		if (grade >= 75 && grade < 78)
			return 2.7f;
		if (grade >= 72 && grade < 75)
			return 2.3f;
		if (grade >= 68 && grade < 72)
			return 2.0f;
		if (grade >= 64 && grade < 68)
			return 1.5f;
		if (grade >= 60 && grade < 64)
			return 1.0f;
		return 0.0f;
	}

	float gpa(const std::vector<std::pair<float, float>>& courses)
	{
		float weighted = 0.0f;
		float credits = 0.0f;
		for (std::vector<std::pair<float, float>>::const_iterator iter = courses.begin(); iter != courses.end(); ++iter)
		{
			const float credit = iter->first;
			const float grade = iter->second;
			credits += credit;
			weighted += credit * gradePoint(grade);
		}
		if (credits == 0.0f)
			return 0.0f;
		return floorToTwoDecimals(weighted / credits);
	}

	float studyScore(float gpa)
	{
		return floorToTwoDecimals(gpa * 25);
	}

	float cappedAddition(float sum)
	{
		return sum > 5.0f ? 5.0f : sum;
	}

	float totalScore(float study, float activity, float moral, float addition)
	{
		const float total = study * 0.7f + activity * 0.15f + moral * 0.15f + addition;
		if (total > 100.0f)
			return 100.0f;
		return floorToTwoDecimals(total);
	}

	std::vector<std::pair<std::string, int>> rankByTotal(const std::vector<std::pair<std::string, float>>& totals)
	{
		std::multimap<float, std::string> ordered;
		for (std::vector<std::pair<std::string, float>>::const_iterator iter = totals.begin(); iter != totals.end(); ++iter)
			ordered.insert(std::pair<float, std::string>(iter->second, iter->first));

		std::vector<std::pair<std::string, int>> ranks;
		int rank = static_cast<int>(ordered.size());
		for (std::multimap<float, std::string>::iterator iter = ordered.begin(); iter != ordered.end(); ++iter, --rank)
			ranks.push_back(std::pair<std::string, int>(iter->second, rank));
		return ranks;
	}
}
