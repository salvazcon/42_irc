#include "Channel.hpp"

Channel::~Channel()
{
    //std::cout << "Channel destructor called" << std::endl;
}

Channel::Channel(): name(""), passwd("")
{
    //std::cout << "Channel constructor called" << std::endl;
}

Channel::Channel(std::string name): name(name), passwd("")
{
    //std::cout << "Channel constructor called" << std::endl;
}

Channel& Channel::operator=(const Channel &other)
{
	//std::cout << "Channel copy assignment operator called" << std::endl;
    this->name = other.name;
    this->passwd = other.passwd;
    this->users = other.users;
	return *this;
}

Channel::Channel(const Channel &cp)
{
	//std::cout << "Channel copy constructor called" << std::endl;
	*this = cp;
}

std::string Channel::getName()
{
    return name;
}

std::string Channel::getPasswd()
{
    return passwd;
}

std::string Channel::getUser(size_t i)
{
    if(users.size() > i)
        return users[i];
    return "";
}

std::vector<std::string>& Channel::getUsers()
{
    return users;
}

void Channel::setName(std::string name)
{
    this->name = name;
}

void Channel::setPasswd(std::string passwd)
{
    this->passwd = passwd;
}

void Channel::setUser(std::string user)
{
    if (!user.empty())
        this->users.push_back(user);
}