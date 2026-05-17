#include "MoralRecord.h"

#include <istream>
#include <ostream>

float sumScores(const MoralRecord& record)
{
	float total = 0.0f;
	for (std::vector<float>::const_iterator iter = record.scores.begin(); iter != record.scores.end(); ++iter)
		total += *iter;
	return total;
}

bool readMoralRecord(std::istream& in, MoralRecord& record, int scoreCount)
{
	if (!(in >> record.receiverAccount >> record.giverAccount))
		return false;
	record.scores.clear();
	for (int index = 0; index < scoreCount; ++index)
	{
		float score = 0.0f;
		if (!(in >> score))
			return false;
		record.scores.push_back(score);
	}
	return true;
}

void writeMoralRecord(std::ostream& out, const MoralRecord& record)
{
	out << record.receiverAccount << '\t' << record.giverAccount;
	for (std::vector<float>::const_iterator iter = record.scores.begin(); iter != record.scores.end(); ++iter)
		out << '\t' << *iter;
	out << '\n';
}
