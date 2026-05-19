#include "ConsoleTable.h"

#include "ConsoleTheme.h"

#include <iostream>

ConsoleTable::Column::Column(const std::string& headerValue, int widthValue)
	: header(headerValue), width(widthValue)
{
}

int ConsoleTable::displayWidth(const std::string& value)
{
	int width = 0;
	for (size_t index = 0; index < value.size();)
	{
		const unsigned char ch = static_cast<unsigned char>(value[index]);
		if (ch < 0x80)
		{
			++width;
			++index;
		}
		else
		{
			width += 2;
			if ((ch & 0xE0) == 0xC0)
				index += 2;
			else if ((ch & 0xF0) == 0xE0)
				index += 3;
			else if ((ch & 0xF8) == 0xF0)
				index += 4;
			else
				++index;
		}
	}
	return width;
}

void ConsoleTable::render(const std::vector<Column>& columns, const std::vector<std::vector<std::string>>& rows)
{
	for (size_t index = 0; index < columns.size(); ++index)
	{
		std::cout << columns[index].header;
		const int spaces = columns[index].width - displayWidth(columns[index].header);
		std::cout << std::string(spaces > 0 ? spaces : 1, ' ');
	}
	std::cout << std::endl;

	if (rows.empty())
	{
		std::cout << ConsoleTheme::emptyData() << std::endl;
		return;
	}

	for (size_t rowIndex = 0; rowIndex < rows.size(); ++rowIndex)
	{
		for (size_t columnIndex = 0; columnIndex < columns.size(); ++columnIndex)
		{
			const std::string value = columnIndex < rows[rowIndex].size() ? rows[rowIndex][columnIndex] : "";
			std::cout << value;
			const int spaces = columns[columnIndex].width - displayWidth(value);
			std::cout << std::string(spaces > 0 ? spaces : 1, ' ');
		}
		std::cout << std::endl;
	}
}
