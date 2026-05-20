#include "LogQueryService.h"

#include <filesystem>
#include <fstream>

namespace
{
	bool matchesLevel(const std::string& line, const std::string& level)
	{
		return level.empty() || line.find("[" + level + "]") != std::string::npos;
	}
}

LogQueryService::LogQueryService(const AssessmentRepository& repository)
	: repository_(repository)
{
}

std::vector<std::string> LogQueryService::recent(const std::string& level, size_t limit) const
{
	std::vector<std::string> lines;
	const std::string path = logFilePath();
	if (!std::filesystem::exists(path))
		return lines;

	std::ifstream input(path);
	std::string line;
	while (std::getline(input, line))
	{
		if (matchesLevel(line, level))
			lines.push_back(line);
	}

	if (lines.size() <= limit)
		return lines;
	return std::vector<std::string>(lines.end() - static_cast<std::ptrdiff_t>(limit), lines.end());
}

std::string LogQueryService::logFilePath() const
{
	return (std::filesystem::path(repository_.runtimeDataDirectory()) / "app.log").string();
}
