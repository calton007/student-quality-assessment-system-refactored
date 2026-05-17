#pragma once

#include <iosfwd>

namespace TextTable
{
	int readRecordCount(std::istream& in);
	void requireNoExtraRecords(std::istream& in);
}
