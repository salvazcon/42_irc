#include "Channel.hpp"

Channel::~Channel(){}

Channel::Channel(): name(""), passwd(""), topicProtected(false), limitChannel(false), inviteOnly(false) {}

Channel::Channel(std::string name): name(name), passwd(""), topicProtected(false), limitChannel(false), inviteOnly(false){}

Channel& Channel::operator=(const Channel &other)
{
    this->name = other.name;
    this->passwd = other.passwd;
    this->users = other.users;
    this->name = other.name;
    this->operators = other.operators;
    this->inviteOnly = other.inviteOnly;
    this->topicProtected = other.topicProtected;
    this->topic = other.topic;
    this->founder = other.founder;
    this->limitChannel = other.limitChannel;
    this->limit = other.limit;
	return *this;
}

Channel::Channel(const Channel &cp)
{
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

std::string Channel::getFounder()
{
    return founder;
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

std::string Channel::getOperator(size_t i)
{
    if(operators.size() > i)
        return operators[i];
    return "";
}

std::vector<std::string>& Channel::getOperators()
{
    return operators;
}

Topic* Channel::getTopic()
{
    return &topic;
}

bool Channel::getTopicProtected()
{
    return topicProtected;
}

bool Channel::getInviteOnly()
{
    return inviteOnly;
}

bool Channel::getLimitChannel()
{
    return limitChannel;
}

size_t Channel::getLimit()
{
    return limit;
}

void Channel::setName(std::string name)
{
    this->name = name;
}

void Channel::setPasswd(std::string passwd)
{
    this->passwd = passwd;
}

void Channel::setFounder(std::string founder)
{
    this->founder = founder;
}

void Channel::setUser(std::string user)
{
    if (!user.empty())
        this->users.push_back(user);
}

void Channel::setOperator(std::string nick)
{
    if (!nick.empty())
        if (std::find(this->operators.begin(), this->operators.end(), nick) == this->operators.end())
            this->operators.push_back(nick);
}

void Channel::setTopic(Topic& topic)
{
    this->topic = topic;
}

void Channel::setInviteOnly(bool inviteOnly)
{
    this->inviteOnly = inviteOnly;
}

void Channel::setTopicProtection(bool topicProtection)
{
    this->topicProtected = topicProtection;
}

void Channel::setLimitChannel(bool limitChannel)
{
    this->limitChannel = limitChannel;
}

void Channel::setLimit(size_t limit)
{
    this->limit = limit;
}

void Channel::removeOperator(const std::string nick)
{
    std::vector<std::string>::iterator it = std::find(operators.begin(), operators.end(), nick);
        if (it != operators.end())
            operators.erase(it);
}

void Channel::removeUser(const std::string user)
{
    std::vector<std::string>::iterator it = std::find(users.begin(), users.end(), user);
        if (it != users.end())
            users.erase(it);
}