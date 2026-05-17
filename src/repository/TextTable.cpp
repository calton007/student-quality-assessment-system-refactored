#include "TextTable.h"

#include <istream>
#include <stdexcept>

namespace
{
	void skipUtf8Bom(std::istream& in)
	{
		if (static_cast<unsigned char>(in.peek()) != 0xEF)
			return;
		in.get();
		if (static_cast<unsigned char>(in.peek()) != 0xBB)
			throw std::runtime_error("invalid UTF-8 BOM");
		in.get();
		if (static_cast<unsigned char>(in.peek()) != 0xBF)
			throw std::runtime_error("invalid UTF-8 BOM");
		in.get();
	}
}

namespace TextTable
{
	int readRecordCount(std::istream& in)
	{
		skipUtf8Bom(in);
		int lines = 0;
		if (!(in >> lines) || lines < 0)
			throw std::runtime_error("invalid record count");
		return lines;
	}

	void requireNoExtraRecords(std::istream& in)
	{
		in >> std::ws;
		if (!in.eof())
			throw std::runtime_error("extra records");
	}
}
