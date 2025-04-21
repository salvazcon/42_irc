#ifndef  CHANNEL_HPP
# define  CHANNEL_HPP

#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <algorithm>

#include "Topic.hpp"

class Channel
{
    protected:

    private:
        std::string name;
        std::string passwd;
        std::string founder;
        std::vector<std::string> users;
        std::vector<std::string> operators;
        bool        topicProtected;
        bool        limitChannel;
        bool        inviteOnly;
        Topic       topic;
        size_t      limit;
    
    public:
        ~Channel(void);
        Channel(void);
        Channel(std::string name);
        Channel(const Channel &cp);
		Channel& operator=(const Channel &other);
        size_t getLimit();
        Topic* getTopic();
        bool getInviteOnly();
        bool getLimitChannel();
        bool getTopicProtected();
        std::string getName();
        std::string getPasswd();
        std::string getFounder();
        std::string getUser(size_t i);
        std::string getOperator(size_t i);
        std::vector<std::string>& getUsers();
        std::vector<std::string>& getOperators();
        void setLimit(size_t limit);
        void setTopic(Topic& topic);
        void setUser(std::string user);
        void setName(std::string name);
        void setPasswd(std::string passwd);
        void setOperator(std::string user);    
        void setInviteOnly(bool inviteOnly);
        void setTopicProtection(bool value);
        void setFounder(std::string founder);
        void setLimitChannel(bool limitChannel);
        void removeUser(const std::string user);
        void removeOperator(const std::string nick);
};

#endif
