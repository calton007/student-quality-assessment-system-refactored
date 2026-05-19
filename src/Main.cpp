#include "AppConfig.h"
#include "AppLogger.h"
#include "AssessmentRepository.h"
#include "ConsoleApp.h"

#include <filesystem>
#include <iostream>
#include <stdexcept>

int main(int argc, char* argv[])
{
	try
	{
		AppConfig config = AppConfig::parse(argc, argv);
		AppLogger logger((std::filesystem::path(config.runtimeDataDirectory()) / "app.log").string());
		try
		{
			logger.info("program started");

			AssessmentRepository repository(config.userFilePath(), config.runtimeDataDirectory(), &logger);
			ConsoleApp app(repository, logger);
			return app.run();
		}
		catch (const std::exception& ex)
		{
			logger.error(std::string("fatal error: ") + ex.what());
			std::cerr << "fatal error: " << ex.what() << std::endl;
			return 1;
		}
	}
	catch (const std::exception& ex)
	{
		std::cerr << "fatal error: " << ex.what() << std::endl;
		return 1;
	}
}
