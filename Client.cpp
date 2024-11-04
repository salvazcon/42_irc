#include "Client.hpp"

Client::~Client()
{
    std::cout << "Client destructor called" << std::endl;
}

Client::Client()
{
    std::cout << "Client constructor called" << std::endl;
}

Client& Client::operator=(const Client &other)
{
    //(<-- Datos a copiar.)
	(void)other;
	std::cout << "Client copy assignment operator called" << std::endl;
	return *this;
}

Client::Client(const Client &cp)
{
	std::cout << "Client copy constructor called" << std::endl;
	*this = cp;
}

int Client::GetFd()
{
	return Fd;
}

void Client::SetFd(int fd)
{
	this->Fd = fd;
}

void Client::setIpAdd(std::string ipadd)
{
	this->IPadd = ipadd;
}