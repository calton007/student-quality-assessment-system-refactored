#pragma once

#include "AssessmentRepository.h"
#include "UserRecord.h"

class StudentConsole
{
public:
	StudentConsole(AssessmentRepository& repository, const UserRecord& user);
	void run();

private:
	void moralMenu();
	void gradeMoral();
	void modifyMoral();
	void activityMenu();
	void newActivity();
	void modifyActivity();
	void deleteActivity();
	void displayActivity();
	void searchMenu();
	void changePassword();

	AssessmentRepository& repository_;
	UserRecord user_;
};
