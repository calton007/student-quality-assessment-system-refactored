#pragma once

#include "AppLogger.h"
#include "AssessmentRepository.h"
#include "UserRecord.h"

#include <string>
#include <vector>

class GroupConsole
{
public:
	GroupConsole(AssessmentRepository& repository, const UserRecord& user, const AppLogger& logger);
	void run();
	static std::vector<std::string> homeMenuItems(bool totalGenerated);

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
	void scoreDetail();
	void exportTotals();
	void backupMenu();
	void logQuery();
	void buildTotal();
	void changePassword();
	bool requireStudent(const std::string& account) const;

	AssessmentRepository& repository_;
	UserRecord user_;
	const AppLogger& logger_;
};
