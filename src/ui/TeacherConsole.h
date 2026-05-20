#pragma once

#include "AssessmentRepository.h"
#include "UserRecord.h"

#include <string>
#include <vector>

class TeacherConsole
{
public:
	TeacherConsole(AssessmentRepository& repository, const UserRecord& user);
	void run();
	static std::vector<std::string> homeMenuItems(bool totalGenerated);

private:
	void gradeMoral();
	void modifyMoral();
	void changePassword();

	AssessmentRepository& repository_;
	UserRecord user_;
};
