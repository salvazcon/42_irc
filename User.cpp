#include "User.hpp"

User::~User()
{
    //std::cout << "User destructor called" << std::endl;
}

User::User(): userName(""), realName(""), userMode(0)
{
    //std::cout << "User constructor called" << std::endl;
}

User::User(std::string user, std::string real, int mode): userName(user), realName(real), userMode(mode)
{
    //std::cout << "User constructor called" << std::endl;
}

User& User::operator=(const User &other)
{
	//std::cout << "User copy assignment operator called" << std::endl;
    this->userName = other.userName;
    this->realName = other.realName;
    this->userMode = other.userMode;
	return *this;
}

User::User(const User &cp)
{
	//std::cout << "User copy constructor called" << std::endl;
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


