#include "Client.hpp"

Client::~Client()
{
    //std::cout << "Client destructor called" << std::endl;
}

Client::Client() : passwd(false), user(), nick(""), /* channels(), */ ip(""), fd(-1)
{
    //std::cout << "Client constructor called" << std::endl;
}

Client& Client::operator=(const Client &other)
{
	//std::cout << "Client copy assignment operator called" << std::endl;
    this->fd = other.fd;
	this->ip = other.ip;
	this->passwd = other.passwd;
	this->nick = other.nick;
	this->user = other.user;
	//this->channels = other.channels;
	return *this;
}

Client::Client(const Client &cp)
{
	//std::cout << "Client copy constructor called" << std::endl;
	*this = cp;
}

int Client::getFd()
{
	return fd;
}

bool Client::getPasswd()
{
	return passwd;
}

std::string Client::getIp()
{
	return ip;
}

std::string Client::getNick()
{
	return nick;
}

User* Client::getUser()
{
	return &user;
}

/* Channel* Client::getChannel(size_t i)
{
	if(channels.size() > i)
		return &channels[i];
	else
		return NULL;
}

std::vector<Channel> Client::getChannels()
{
	return channels;
} */

void Client::setFd(int fd)
{
	this->fd = fd;
}

void Client::setIp(std::string Ip)
{
	this->ip = Ip;
}

void Client::setPasswd(bool passwd)
{
	this->passwd = passwd;
}

void Client::setNick(std::string nick)
{
	this->nick = nick;
}

void Client::setUser(User& user)
{
	this->user = user;
}

/* void Client::setChannel()
{

} */