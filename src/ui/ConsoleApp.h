#pragma once

#include "AppLogger.h"
#include "AssessmentRepository.h"
#include "UserRecord.h"

class ConsoleApp
{
public:
	ConsoleApp(AssessmentRepository& repository, const AppLogger& logger);
	int run();

private:
	UserRecord login();
	void openMenu(const UserRecord& user);

	AssessmentRepository& repository_;
	const AppLogger& logger_;
};
