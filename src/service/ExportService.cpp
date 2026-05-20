#include "ExportService.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace
{
	std::string csvCell(const std::string& value)
	{
		bool needsQuotes = false;
		for (std::string::const_iterator iter = value.begin(); iter != value.end(); ++iter)
		{
			if (*iter == ',' || *iter == '"' || *iter == '\n' || *iter == '\r')
			{
				needsQuotes = true;
				break;
			}
		}
		if (!needsQuotes)
			return value;

		std::string escaped = "\"";
		for (std::string::const_iterator iter = value.begin(); iter != value.end(); ++iter)
		{
			if (*iter == '"')
				escaped += "\"\"";
			else
				escaped += *iter;
		}
		escaped += '"';
		return escaped;
	}

	template <typename T>
	std::string toText(const T& value)
	{
		std::ostringstream output;
		output << value;
		return output.str();
	}
}

ExportService::ExportService(const AssessmentRepository& repository)
	: repository_(repository)
{
}

std::string ExportService::exportTotalsCsv() const
{
	if (!repository_.status().totalGenerated)
		throw std::runtime_error("尚未生成综测成绩,无法导出!");

	const std::filesystem::path path = std::filesystem::path(repository_.runtimeDataDirectory()) / "TotalExport.csv";
	std::ofstream output(path, std::ios::binary);
	if (!output)
		throw std::runtime_error("cannot write export file: " + path.string());
	output << "\xEF\xBB\xBF";
	output << buildCsv();
	if (!output)
		throw std::runtime_error("cannot write export file: " + path.string());
	return path.string();
}

std::string ExportService::buildCsv() const
{
	std::ostringstream output;
	output << "学号,姓名,学习成绩,GPA,课外活动成绩,思想品德成绩,附加分,综测成绩,排名\n";
	for (std::map<std::string, StudentScore>::const_iterator iter = repository_.totals().begin(); iter != repository_.totals().end(); ++iter)
	{
		std::map<std::string, UserRecord>::const_iterator user = repository_.users().find(iter->first);
		const std::string name = user == repository_.users().end() ? "" : user->second.name;
		const StudentScore& score = iter->second;
		output
			<< csvCell(score.account) << ','
			<< csvCell(name) << ','
			<< toText(score.study) << ','
			<< toText(score.gpa) << ','
			<< toText(score.activity) << ','
			<< toText(score.moral) << ','
			<< toText(score.addition) << ','
			<< toText(score.total) << ','
			<< toText(score.rank) << '\n';
	}
	return output.str();
}
