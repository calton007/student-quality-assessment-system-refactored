#include "AppLogger.h"

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace
{
	std::string timestamp()
	{
		const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::tm localTime;
		if (localtime_s(&localTime, &now) != 0)
			throw std::runtime_error("cannot build log timestamp");
		std::ostringstream output;
		output << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
		return output.str();
	}
}

AppLogger::AppLogger(const std::string& logFilePath)
	: logFilePath_(logFilePath)
{
}

void AppLogger::info(const std::string& message) const
{
	write("INFO", message);
}

void AppLogger::error(const std::string& message) const
{
	write("ERROR", message);
}

void AppLogger::write(const std::string& level, const std::string& message) const
{
	std::ofstream output(logFilePath_, std::ios::app);
	if (!output)
		throw std::runtime_error("cannot write log file: " + logFilePath_);
	output << timestamp() << " [" << level << "] " << message << '\n';
	if (!output)
		throw std::runtime_error("cannot write log file: " + logFilePath_);
}
