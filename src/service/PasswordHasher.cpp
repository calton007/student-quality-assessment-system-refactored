#include "PasswordHasher.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bcrypt.h>

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace
{
	const char* Prefix = "PBKDF2-SHA256";
	const int Iterations = 100000;
	const int SaltBytes = 16;
	const int HashBytes = 32;

	void requireOk(NTSTATUS status, const char* message)
	{
		if (status < 0)
			throw std::runtime_error(message);
	}

	std::string toHex(const std::vector<unsigned char>& bytes)
	{
		std::ostringstream output;
		for (size_t index = 0; index < bytes.size(); ++index)
			output << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(bytes[index]);
		return output.str();
	}

	std::vector<unsigned char> fromHex(const std::string& text)
	{
		if (text.size() % 2 != 0)
			throw std::runtime_error("invalid password hash");
		std::vector<unsigned char> bytes;
		for (size_t index = 0; index < text.size(); index += 2)
		{
			unsigned int value = 0;
			std::istringstream input(text.substr(index, 2));
			if (!(input >> std::hex >> value))
				throw std::runtime_error("invalid password hash");
			bytes.push_back(static_cast<unsigned char>(value));
		}
		return bytes;
	}

	std::vector<std::string> split(const std::string& value)
	{
		std::vector<std::string> parts;
		size_t start = 0;
		while (true)
		{
			const size_t found = value.find('$', start);
			if (found == std::string::npos)
			{
				parts.push_back(value.substr(start));
				return parts;
			}
			parts.push_back(value.substr(start, found - start));
			start = found + 1;
		}
	}

	std::vector<unsigned char> derive(const std::string& password, const std::vector<unsigned char>& salt, int iterations)
	{
		BCRYPT_ALG_HANDLE algorithm = NULL;
		requireOk(BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, NULL, BCRYPT_ALG_HANDLE_HMAC_FLAG), "cannot open password hash provider");
		std::vector<unsigned char> result(HashBytes);
		NTSTATUS status = BCryptDeriveKeyPBKDF2(
			algorithm,
			reinterpret_cast<PUCHAR>(const_cast<char*>(password.data())),
			static_cast<ULONG>(password.size()),
			const_cast<PUCHAR>(salt.data()),
			static_cast<ULONG>(salt.size()),
			iterations,
			result.data(),
			static_cast<ULONG>(result.size()),
			0);
		BCryptCloseAlgorithmProvider(algorithm, 0);
		requireOk(status, "cannot derive password hash");
		return result;
	}
}

namespace PasswordHasher
{
	std::string hash(const std::string& password)
	{
		if (password.empty())
			throw std::runtime_error("密码不能为空");
		std::vector<unsigned char> salt(SaltBytes);
		requireOk(BCryptGenRandom(NULL, salt.data(), static_cast<ULONG>(salt.size()), BCRYPT_USE_SYSTEM_PREFERRED_RNG), "cannot generate password salt");
		const std::vector<unsigned char> derived = derive(password, salt, Iterations);
		std::ostringstream output;
		output << Prefix << '$' << Iterations << '$' << toHex(salt) << '$' << toHex(derived);
		return output.str();
	}

	bool verify(const std::string& password, const std::string& storedHash)
	{
		const std::vector<std::string> parts = split(storedHash);
		if (parts.size() != 4 || parts[0] != Prefix)
			return false;
		const int iterations = std::stoi(parts[1]);
		const std::vector<unsigned char> salt = fromHex(parts[2]);
		const std::vector<unsigned char> expected = fromHex(parts[3]);
		const std::vector<unsigned char> actual = derive(password, salt, iterations);
		if (actual.size() != expected.size())
			return false;
		unsigned char diff = 0;
		for (size_t index = 0; index < actual.size(); ++index)
			diff |= actual[index] ^ expected[index];
		return diff == 0;
	}

	bool isHash(const std::string& value)
	{
		return value.find(std::string(Prefix) + "$") == 0;
	}
}
