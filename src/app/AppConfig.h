#pragma once

#include <string>

class AppConfig
{
public:
	static AppConfig parse(int argc, char* argv[]);

	const std::string& userFilePath() const;
	const std::string& runtimeDataDirectory() const;

private:
	AppConfig(const std::string& userFilePath, const std::string& runtimeDataDirectory);

	std::string userFilePath_;
	std::string runtimeDataDirectory_;
};
