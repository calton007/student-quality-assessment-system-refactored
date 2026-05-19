#pragma once

#include <string>
#include <vector>

class ConsoleTable
{
public:
	struct Column
	{
		Column(const std::string& header, int width);

		std::string header;
		int width;
	};

	static int displayWidth(const std::string& value);
	static void render(const std::vector<Column>& columns, const std::vector<std::vector<std::string>>& rows);
};
