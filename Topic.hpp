#ifndef  TOPIC_HPP
# define  TOPIC_HPP

#include <iostream>
#include <string>

class Topic
{
    protected:

    private:
        std::string topic;
        std::string author;

    public:
        ~Topic(void);
        Topic(void);
        Topic(std::string topic, std::string author);
        Topic(const Topic &cp);
		Topic& operator=(const Topic &other);
        std::string getTopic();
        std::string getAuthor();
        void setTopic(bool topic);
        void setAuthor(bool author);
};

#endif
