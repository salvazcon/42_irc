#include "Client.hpp"

Client::~Client()
{
    //std::cout << "Client destructor called" << std::endl;
}

Client::Client()
{
    //std::cout << "Client constructor called" << std::endl;
}

Client& Client::operator=(const Client &other)
{
    this->Fd = other.Fd;
	this->Ip = other.Ip;
	//std::cout << "Client copy assignment operator called" << std::endl;
	return *this;
}

Client::Client(const Client &cp)
{
	//std::cout << "Client copy constructor called" << std::endl;
	*this = cp;
}

int Client::GetFd()
{
	return Fd;
}

std::string Client::GetIp()
{
	return Ip;
}

void Client::SetFd(int fd)
{
	this->Fd = fd;
}

void Client::setIp(std::string Ip)
{
	this->Ip = Ip;
}