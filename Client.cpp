#include "Client.hpp"

Client::~Client()
{
    //std::cout << "Client destructor called" << std::endl;
	this->CleanChannels();
}

Client::Client() : passwd(false), user(), channels(), ip(""), fd(-1)
{
    //std::cout << "Client constructor called" << std::endl;
}

Client& Client::operator=(const Client& other)
{
    if (this != &other) {
        for (size_t i = 0; i < channels.size(); ++i) {
            delete channels[i];
        }
        channels.clear();
        std::vector<Channel*> tempChannels;
        for (size_t i = 0; i < other.channels.size(); ++i) {
            Channel* newChannel = new Channel(*other.channels[i]);
            if (!newChannel) {
                for (size_t j = 0; j < tempChannels.size(); ++j) {
                    delete tempChannels[j];
                }
                tempChannels.clear();
                return *this;
            }
            tempChannels.push_back(newChannel);
        }
        this->fd = other.fd;
        this->ip = other.ip;
        this->passwd = other.passwd;
        this->nick = other.nick;
        this->user = other.user;
        channels.swap(tempChannels);
    }
    return *this;
}

Client::Client(const Client &cp): channels()
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

Channel* Client::getChannel(size_t i)
{
	if(channels.size() > i)
		return channels[i];
	return NULL;
}

std::vector<Channel*> Client::getChannels()
{
	return channels;
}

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

void Client::setChannel(Channel* channel)
{
	if (channel != NULL)
        this->channels.push_back(channel);
}

void Client::CleanChannels()
{
	for (size_t i = 0; i < this->channels.size();)
	{
		if(this->channels[i]->getUsers().size() == 1)
		{
			delete channels[i];
			channels.erase(this->channels.begin() + i);
		}
		else
		{
			for (size_t j = 0; j < this->channels[i]->getUsers().size(); j++)
			{
				if(this->nick == this->channels[i]->getUser(j))
				{
					this->channels[i]->getUsers().erase(this->channels[i]->getUsers().begin() + j);
					break ;
				}
			}
			++i;
		}
	}
	channels.clear();
}