#include "StudentScore.h"

#include <istream>
#include <ostream>

std::istream& operator>>(std::istream& in, StudentScore& score)
{
	in >> score.account >> score.study >> score.gpa >> score.activity >> score.moral
		>> score.addition >> score.total >> score.rank;
	return in;
}

std::ostream& operator<<(std::ostream& out, const StudentScore& score)
{
	out << score.account << '\t' << score.study << '\t' << score.gpa << '\t' << score.activity << '\t'
		<< score.moral << '\t' << score.addition << '\t' << score.total << '\t' << score.rank << '\n';
	return out;
}
