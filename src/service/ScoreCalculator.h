#pragma once

#include <string>
#include <utility>
#include <vector>

namespace ScoreCalculator
{
	float gradePoint(float grade);
	float gpa(const std::vector<std::pair<float, float>>& courses);
	float studyScore(float gpa);
	float cappedAddition(float sum);
	float totalScore(float study, float activity, float moral, float addition);
	std::vector<std::pair<std::string, int>> rankByTotal(const std::vector<std::pair<std::string, float>>& totals);
}
