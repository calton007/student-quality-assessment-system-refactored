#pragma once

#include <iosfwd>
#include <string>
#include <vector>

struct MoralRecord
{
	std::string receiverAccount;
	std::string giverAccount;
	std::vector<float> scores;
};

float sumScores(const MoralRecord& record);
bool readMoralRecord(std::istream& in, MoralRecord& record, int scoreCount);
void writeMoralRecord(std::ostream& out, const MoralRecord& record);
