#pragma once

#include "AssessmentRepository.h"
#include "UserRecord.h"

#include <string>

class AuthService
{
public:
	explicit AuthService(AssessmentRepository& repository);

	UserRecord login(const std::string& account, const std::string& password) const;
	void changePassword(const std::string& account, const std::string& oldPassword, const std::string& newPassword);

private:
	AssessmentRepository& repository_;
};
