#pragma once

#include "AssessmentRepository.h"
#include "UserRecord.h"

class TeacherConsole
{
public:
	TeacherConsole(AssessmentRepository& repository, const UserRecord& user);
	void run();

private:
	void gradeMoral();
	void modifyMoral();
	void changePassword();

	AssessmentRepository& repository_;
	UserRecord user_;
};
