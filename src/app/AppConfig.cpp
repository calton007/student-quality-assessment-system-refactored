#include "AppConfig.h"

#include <cstdlib>
#include <filesystem>
#include <stdexcept>
#include <string>

namespace
{
	std::string defaultRuntimeDataDirectory()
	{
		char* localAppData = NULL;
		size_t length = 0;
		if (_dupenv_s(&localAppData, &length, "LOCALAPPDATA") != 0 || localAppData == NULL || std::string(localAppData).empty())
		{
			std::free(localAppData);
			throw std::runtime_error("LOCALAPPDATA is not set");
		}
		const std::string value = localAppData;
		std::free(localAppData);
		return (std::filesystem::path(value) / "StudentQualityAssessment").string();
	}
}

AppConfig::AppConfig(const std::string& userFilePath, const std::string& runtimeDataDirectory)
	: userFilePath_(userFilePath), runtimeDataDirectory_(runtimeDataDirectory)
{
}

AppConfig AppConfig::parse(int argc, char* argv[])
{
	std::string runtimeDataDirectory = defaultRuntimeDataDirectory();
	for (int index = 1; index < argc; ++index)
	{
		const std::string argument = argv[index];
		if (argument == "--data-dir")
		{
			if (index + 1 >= argc)
				throw std::runtime_error("--data-dir requires a path");
			runtimeDataDirectory = argv[++index];
			if (runtimeDataDirectory.empty())
				throw std::runtime_error("--data-dir requires a path");
			continue;
		}
		throw std::runtime_error("unknown argument: " + argument);
	}

	std::filesystem::create_directories(runtimeDataDirectory);
	if (!std::filesystem::is_directory(runtimeDataDirectory))
		throw std::runtime_error("cannot create data directory: " + runtimeDataDirectory);

	return AppConfig("User.txt", runtimeDataDirectory);
}

const std::string& AppConfig::userFilePath() const
{
	return userFilePath_;
}

const std::string& AppConfig::runtimeDataDirectory() const
{
	return runtimeDataDirectory_;
}
