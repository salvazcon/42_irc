#include "Client.hpp"

Client::~Client()
{
    //std::cout << "Client destructor called" << std::endl;
}

Client::Client(): Passwd(false)
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

bool Client::getPasswd()
{
	return Passwd;
}

int Client::getFd()
{
	return Fd;
}

std::string Client::getIp()
{
	return Ip;
}

std::string Client::getUser()
{
	return Username;
}

std::string Client::getNick()
{
	return Nickname;
}

void Client::setFd(int fd)
{
	this->Fd = fd;
}

void Client::setIp(std::string Ip)
{
	this->Ip = Ip;
}

void Client::setUser(std::string user)
{
	this->Username = user;
}

void Client::setNick(std::string nick)
{
	this->Nickname = nick;
}

void Client::setPasswd(bool passwd)
{
	this->Passwd = passwd;
}