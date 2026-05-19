#include "SecureFileStore.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dpapi.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace
{
	const char* Header = "SQAS-DPAPI-1";

	std::string readAll(const std::string& path)
	{
		std::ifstream input(path, std::ios::binary);
		if (!input)
			throw std::runtime_error("cannot open data file: " + path);
		std::ostringstream output;
		output << input.rdbuf();
		return output.str();
	}

	std::string toHex(const std::vector<unsigned char>& bytes)
	{
		static const char* digits = "0123456789abcdef";
		std::string text;
		for (size_t index = 0; index < bytes.size(); ++index)
		{
			text.push_back(digits[bytes[index] >> 4]);
			text.push_back(digits[bytes[index] & 0x0F]);
		}
		return text;
	}

	unsigned char hexValue(char ch)
	{
		if (ch >= '0' && ch <= '9')
			return static_cast<unsigned char>(ch - '0');
		if (ch >= 'a' && ch <= 'f')
			return static_cast<unsigned char>(ch - 'a' + 10);
		if (ch >= 'A' && ch <= 'F')
			return static_cast<unsigned char>(ch - 'A' + 10);
		throw std::runtime_error("invalid protected data file");
	}

	std::vector<unsigned char> fromHex(const std::string& text)
	{
		if (text.size() % 2 != 0)
			throw std::runtime_error("invalid protected data file");
		std::vector<unsigned char> bytes;
		for (size_t index = 0; index < text.size(); index += 2)
			bytes.push_back(static_cast<unsigned char>((hexValue(text[index]) << 4) | hexValue(text[index + 1])));
		return bytes;
	}

	void writeAtomic(const std::string& path, const std::string& content)
	{
		const std::string tempPath = path + ".tmp";
		const std::string backupPath = path + ".bak";
		if (std::filesystem::exists(path))
		{
			if (!CopyFileA(path.c_str(), backupPath.c_str(), FALSE))
				throw std::runtime_error("cannot backup data file: " + path);
		}
		{
			std::ofstream output(tempPath, std::ios::binary | std::ios::trunc);
			if (!output)
				throw std::runtime_error("cannot write data file: " + tempPath);
			output.write(content.data(), static_cast<std::streamsize>(content.size()));
			if (!output)
				throw std::runtime_error("cannot write data file: " + tempPath);
		}
		if (!MoveFileExA(tempPath.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
			throw std::runtime_error("cannot replace data file: " + path);
	}

	std::string protectText(const std::string& content)
	{
		DATA_BLOB plain;
		plain.pbData = reinterpret_cast<BYTE*>(const_cast<char*>(content.data()));
		plain.cbData = static_cast<DWORD>(content.size());

		DATA_BLOB protectedData;
		if (!CryptProtectData(&plain, L"SQAS runtime data", NULL, NULL, NULL, 0, &protectedData))
			throw std::runtime_error("cannot protect data file");
		std::vector<unsigned char> bytes(protectedData.pbData, protectedData.pbData + protectedData.cbData);
		LocalFree(protectedData.pbData);

		return std::string(Header) + "\n" + toHex(bytes) + "\n";
	}

	std::string unprotectText(const std::string& text)
	{
		std::istringstream input(text);
		std::string header;
		std::string payload;
		std::getline(input, header);
		std::getline(input, payload);
		if (header != Header)
			throw std::runtime_error("invalid protected data file");

		std::vector<unsigned char> bytes = fromHex(payload);
		DATA_BLOB protectedData;
		protectedData.pbData = bytes.data();
		protectedData.cbData = static_cast<DWORD>(bytes.size());

		DATA_BLOB plain;
		if (!CryptUnprotectData(&protectedData, NULL, NULL, NULL, NULL, 0, &plain))
			throw std::runtime_error("cannot unprotect data file");
		std::string content(reinterpret_cast<char*>(plain.pbData), reinterpret_cast<char*>(plain.pbData) + plain.cbData);
		LocalFree(plain.pbData);
		return content;
	}
}

namespace SecureFileStore
{
	bool readOptionalProtectedText(const std::string& path, std::string& content)
	{
		if (!std::filesystem::exists(path))
			return false;
		const std::string text = readAll(path);
		if (text.find(std::string(Header) + "\n") == 0)
			content = unprotectText(text);
		else
			content = text;
		return true;
	}

	void writeProtectedText(const std::string& path, const std::string& content)
	{
		writeAtomic(path, protectText(content));
	}

	void writePlainText(const std::string& path, const std::string& content)
	{
		writeAtomic(path, content);
	}
}
