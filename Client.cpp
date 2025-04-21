#include "Client.hpp"

Client::~Client()
{
	this->CleanChannels();
}

Client::Client() : passwd(false), user(), channels(), ip(""), fd(-1){}

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
        this->invitations = other.invitations;
        channels.swap(tempChannels);
    }
    return *this;
}

Client::Client(const Client &cp): channels()
{
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
    if (channels.size() > i)
    {
        return channels[i];
    }
	return NULL;
}

std::vector<Channel*>& Client::getChannels()
{
    return channels;
}

std::vector<std::string> Client::getInvitations()
{
    return invitations;
}

bool Client::getInvitation(std::string channel)
{
    for (size_t i = 0; i < invitations.size(); ++i)
    {
        if (invitations[i] == channel)
            return true;
    }
    return false;
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

int Client::setInvitation(std::string channel)
{
    if (!channel.empty())
    {
        if (std::find(this->invitations.begin(), this->invitations.end(), channel) == this->invitations.end())
        {
            this->invitations.push_back(channel);
            return 1;
        }    
    }
    return 0;
}

void Client::removeInvitation(std::string channel)
{
    for (size_t i = 0; i < invitations.size(); ++i)
    {
        if (invitations[i] == channel)
        {
            invitations.erase(invitations.begin() + i);
            break;
        }
    }
}

void  Client::removeChannel(Channel *channel)
{
    std::vector<Channel*>::iterator it = std::find(channels.begin(), channels.end(), channel);
    if (it != channels.end()) 
    {
        channel->removeUser(this->nick);
        channel->removeOperator(this->nick);
        channels.erase(it);
        if (channel->getUsers().empty()) 
            delete channel;
    }
}

void Client::CleanChannels()
{
    for (size_t i = 0; i < this->channels.size();)
    {
        Channel* channel = this->channels[i];

        channel->removeUser(this->nick);
        channel->removeOperator(this->nick);
        if (channel->getUsers().empty())
        {
            delete channel;
            channels.erase(channels.begin() + i);
        }
        else
            ++i;
    }
}