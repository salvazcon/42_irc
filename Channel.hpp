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

class Channel
{
    protected:

    private:
        std::string name;
        std::string passwd;
        std::vector<std::string> users;
        //std::string topic;
    
    public:
        ~Channel(void);
        Channel(void);
        Channel(const Channel &cp);
		Channel& operator=(const Channel &other);
        std::string getName();
        std::string getPasswd();
        void setName(std::string name);
        void setPasswd(std::string passwd);
};

#endif
