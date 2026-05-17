#pragma once

#include "AssessmentRepository.h"
#include "UserRecord.h"

class ConsoleApp
{
public:
	explicit ConsoleApp(AssessmentRepository& repository);
	int run();

private:
	UserRecord login();
	void openMenu(const UserRecord& user);

	AssessmentRepository& repository_;
};
