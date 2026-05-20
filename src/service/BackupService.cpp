#include "BackupService.h"

#include <chrono>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace
{
	std::string timestamp()
	{
		const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::tm localTime;
		if (localtime_s(&localTime, &now) != 0)
			throw std::runtime_error("cannot build backup timestamp");
		std::ostringstream output;
		output << std::put_time(&localTime, "%Y%m%d-%H%M%S");
		return output.str();
	}
}

BackupService::BackupService(const AssessmentRepository& repository)
	: repository_(repository)
{
}

std::vector<BackupEntry> BackupService::listBackups() const
{
	std::vector<BackupEntry> entries;
	const std::filesystem::path directory = backupsDirectory();
	if (!std::filesystem::exists(directory))
		return entries;
	for (std::filesystem::directory_iterator iter(directory); iter != std::filesystem::directory_iterator(); ++iter)
	{
		if (!iter->is_directory())
			continue;
		BackupEntry entry;
		entry.name = iter->path().filename().string();
		entry.path = iter->path().string();
		entries.push_back(entry);
	}
	return entries;
}

std::string BackupService::createBackup(const std::string& prefix) const
{
	const std::string target = uniqueBackupDirectory(prefix);
	std::filesystem::create_directories(target);
	copyDirectoryContents(repository_.runtimeDataDirectory(), target, true);
	return target;
}

std::string BackupService::restoreBackup(const std::string& backupName) const
{
	const std::filesystem::path backupPath = std::filesystem::path(backupsDirectory()) / backupName;
	if (!std::filesystem::is_directory(backupPath))
		throw std::runtime_error("备份不存在: " + backupName);

	const std::string safetyBackup = createBackup("pre-restore");
	const std::filesystem::path runtimePath = repository_.runtimeDataDirectory();
	const std::filesystem::path backupsPath = backupsDirectory();
	for (std::filesystem::directory_iterator iter(runtimePath); iter != std::filesystem::directory_iterator(); ++iter)
	{
		if (iter->path() == backupsPath)
			continue;
		std::filesystem::remove_all(iter->path());
	}
	copyDirectoryContents(backupPath.string(), runtimePath.string(), false);
	return safetyBackup;
}

std::string BackupService::backupsDirectory() const
{
	return (std::filesystem::path(repository_.runtimeDataDirectory()) / "backups").string();
}

std::string BackupService::uniqueBackupDirectory(const std::string& prefix) const
{
	std::filesystem::create_directories(backupsDirectory());
	const std::string baseName = prefix + "-" + timestamp();
	std::filesystem::path path = std::filesystem::path(backupsDirectory()) / baseName;
	int suffix = 1;
	while (std::filesystem::exists(path))
		path = std::filesystem::path(backupsDirectory()) / (baseName + "-" + std::to_string(suffix++));
	return path.string();
}

void BackupService::copyDirectoryContents(const std::string& source, const std::string& target, bool skipBackups) const
{
	const std::filesystem::path sourcePath = source;
	const std::filesystem::path targetPath = target;
	const std::filesystem::path backupsPath = backupsDirectory();
	for (std::filesystem::recursive_directory_iterator iter(sourcePath); iter != std::filesystem::recursive_directory_iterator(); ++iter)
	{
		if (skipBackups && iter->path() == backupsPath)
		{
			iter.disable_recursion_pending();
			continue;
		}
		const std::filesystem::path relative = std::filesystem::relative(iter->path(), sourcePath);
		const std::filesystem::path destination = targetPath / relative;
		if (iter->is_directory())
			std::filesystem::create_directories(destination);
		else if (iter->is_regular_file())
		{
			std::filesystem::create_directories(destination.parent_path());
			std::filesystem::copy_file(iter->path(), destination, std::filesystem::copy_options::overwrite_existing);
		}
	}
}
