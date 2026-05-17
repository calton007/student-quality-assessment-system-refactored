#include "AuthService.h"

#include "PasswordHasher.h"

#include <stdexcept>

AuthService::AuthService(AssessmentRepository& repository)
	: repository_(repository)
{
}

UserRecord AuthService::login(const std::string& account, const std::string& password) const
{
	std::map<std::string, UserRecord>::const_iterator iter = repository_.users().find(account);
	if (iter == repository_.users().end() || !PasswordHasher::verify(password, iter->second.password))
		throw std::runtime_error("用户名或密码错误");
	return iter->second;
}

void AuthService::changePassword(const std::string& account, const std::string& oldPassword, const std::string& newPassword)
{
	std::map<std::string, UserRecord>::iterator iter = repository_.users().find(account);
	if (iter == repository_.users().end())
		throw std::runtime_error("用户不存在");
	if (!PasswordHasher::verify(oldPassword, iter->second.password))
		throw std::runtime_error("原密码错误");
	if (oldPassword == newPassword)
		throw std::runtime_error("新密码不能与原密码相同");
	iter->second.password = PasswordHasher::hash(newPassword);
	repository_.saveUsers();
}
