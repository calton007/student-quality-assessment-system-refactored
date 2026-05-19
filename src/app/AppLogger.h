#pragma once

#include <string>

class AppLogger
{
public:
	explicit AppLogger(const std::string& logFilePath);

	void info(const std::string& message) const;
	void error(const std::string& message) const;

private:
	void write(const std::string& level, const std::string& message) const;

	std::string logFilePath_;
};
