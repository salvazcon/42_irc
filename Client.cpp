#include "Client.hpp"

Client::~Client()
{
    //std::cout << "Client destructor called" << std::endl;
}

Client::Client() : Passwd(false), Nickname(""), Username(""), Realname(""), Ip(""), Usermode(0), Fd(-1)
{
    //std::cout << "Client constructor called" << std::endl;
}

Client& Client::operator=(const Client &other)
{
	//std::cout << "Client copy assignment operator called" << std::endl;
    this->Fd = other.Fd;
	this->Ip = other.Ip;
	this->Username = other.Username;
	this->Nickname = other.Nickname;
	this->Realname = other.Realname;
	this->Usermode = other.Usermode;
	this->Passwd = other.Passwd;
	return *this;
}

Client::Client(const Client &cp)
{
	//std::cout << "Client copy constructor called" << std::endl;
	*this = cp;
}

int Client::getFd()
{
	return Fd;
}

int Client::getUserM()
{
	return Usermode;
}

bool Client::getPasswd()
{
	return Passwd;
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

std::string Client::getRealN()
{
	return Realname;
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

void Client::setRealN(std::string real)
{
	this->Realname = real;
}

void Client::setUserM(int n)
{
	this->Usermode = n;
}

void Client::setNick(std::string nick)
{
	this->Nickname = nick;
}

void Client::setPasswd(bool passwd)
{
	this->Passwd = passwd;
}