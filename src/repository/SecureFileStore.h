#pragma once

#include <string>

namespace SecureFileStore
{
	bool readOptionalProtectedText(const std::string& path, std::string& content);
	void writeProtectedText(const std::string& path, const std::string& content);
	void writePlainText(const std::string& path, const std::string& content);
}
