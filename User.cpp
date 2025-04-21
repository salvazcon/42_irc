#include "User.hpp"

User::~User(){}

User::User(): userName(""), realName(""), userMode(0){}

User::User(std::string user, std::string real, int mode): userName(user), realName(real), userMode(mode){}

User& User::operator=(const User &other)
{
    this->userName = other.userName;
    this->realName = other.realName;
    this->userMode = other.userMode;
	return *this;
}

User::User(const User &cp)
{
	*this = cp;
}

std::string User::getUserName()
{
	return userName;
}

std::string User::getRealName()
{
	return realName;
}

int User::getUserMode()
{
	return userMode;
}

void User::setUserName(std::string user)
{
	this->userName = user;
}

void User::setRealName(std::string real)
{
	this->realName = real;
}

void User::setUserMode(int n)
{
	this->userMode = n;
}


