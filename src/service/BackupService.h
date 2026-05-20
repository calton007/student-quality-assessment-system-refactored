#pragma once

#include "AssessmentRepository.h"

#include <string>
#include <vector>

struct BackupEntry
{
	std::string name;
	std::string path;
};

class BackupService
{
public:
	explicit BackupService(const AssessmentRepository& repository);

	std::vector<BackupEntry> listBackups() const;
	std::string createBackup(const std::string& prefix = "sqas-backup") const;
	std::string restoreBackup(const std::string& backupName) const;

private:
	std::string backupsDirectory() const;
	std::string uniqueBackupDirectory(const std::string& prefix) const;
	void copyDirectoryContents(const std::string& source, const std::string& target, bool skipBackups) const;

	const AssessmentRepository& repository_;
};
