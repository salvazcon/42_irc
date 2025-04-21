#include "Topic.hpp"

Topic::~Topic(){}

Topic::Topic(): topic(), author(){}

Topic::Topic(std::string topic, std::string author): topic(topic), author(author){}

Topic& Topic::operator=(const Topic &other)
{
    this->topic = other.topic;
    this->author = other.author;
	return *this;
}

Topic::Topic(const Topic &cp)
{
	*this = cp;
}

std::string Topic::getAuthor()
{
	return author;
}

std::string Topic::getTopic()
{
    return topic;
}

void Topic::setAuthor(bool author)
{
    this->author = author;
}

void Topic::setTopic(bool topic)
{
	this->topic = topic;
}
