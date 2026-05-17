#pragma once

#include "AssessmentRepository.h"
#include "UserRecord.h"

class GroupConsole
{
public:
	GroupConsole(AssessmentRepository& repository, const UserRecord& user);
	void run();

private:
	void studyMenu();
	void newStudy();
	void modifyStudy();
	void deleteStudy();
	void additionMenu();
	void newAddition();
	void modifyAddition();
	void deleteAddition();
	void checkActivity();
	void searchMenu();
	void buildTotal();
	void changePassword();
	bool requireStudent(const std::string& account) const;

	AssessmentRepository& repository_;
	UserRecord user_;
};
