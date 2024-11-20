#ifndef  CLIENT_HPP
# define  CLIENT_HPP

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

#include "User.hpp"
#include "Channel.hpp"

//-------------------------------------------------------//
#define RED "\e[1;31m" //-> for red color
#define WHI "\e[0;37m" //-> for white color
#define GRE "\e[1;32m" //-> for green color
#define YEL "\e[1;33m" //-> for yellow color
//-------------------------------------------------------//

class Client
{
    protected:

    private:
        bool                    passwd;
        User                    user;
        std::string             nick;
        /* std::vector<Channel>    channels; */
        std::string ip;
        int fd;

    public:
        ~Client(void);
        Client(void);
        Client(const Client &cp);
        Client& operator=(const Client &other);
        int getFd();
        bool getPasswd();
        std::string getIp();
        std::string getNick();
        User* getUser();
        Channel* getChannel(size_t i);
        std::vector<Channel> getChannels();
        void setFd(int fd);
        void setIp(std::string ipadd);
        void setPasswd(bool passwd);
        void setNick(std::string nick);
        void setUser(User& user);
};
#endif
