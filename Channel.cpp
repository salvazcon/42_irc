#include "Channel.hpp"

Channel::~Channel()
{
    //std::cout << "Channel destructor called" << std::endl;
}

Channel::Channel()
{
    //std::cout << "Channel constructor called" << std::endl;
}

Channel& Channel::operator=(const Channel &other)
{
	//std::cout << "Channel copy assignment operator called" << std::endl;
    (void)other;
    //(<-- Datos a copiar.)
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

void Channel::setName(std::string name)
{
    this->name = name;
}

void Channel::setPasswd(std::string passwd)
{
    this->passwd = passwd;
}