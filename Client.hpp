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

//-------------------------//
#define RED "\e[1;31m" //-> for red color
#define WHI "\e[0;37m" //-> for white color
#define GRE "\e[1;32m" //-> for green color
#define YEL "\e[1;33m" //-> for yellow color
//-------------------------//

class Client
{
    protected:

    private:
        bool                        passwd;
        User                        user;
        std::string                 nick;
        std::string                 inputBuffer;
        std::vector<Channel*>       channels;
        std::vector<std::string>    invitations;
        std::string                 ip;
        int                         fd;

    public:
        ~Client(void);
        Client(void);
        Client(const Client &cp);
        Client& operator=(const Client &other);
        int getFd();
        bool getPasswd();
        std::string getIp();
        std::string getNick();
        std::string& getInputBuffer();
        User* getUser();
        Channel* getChannel(size_t i);
        bool getInvitation(std::string channel);
        std::vector<Channel*>& getChannels();
        std::vector<std::string> getInvitations();
        int setInvitation(std::string channel);
        void setFd(int fd);
        void setUser(User& user);
        void setPasswd(bool passwd);
        void setIp(std::string ipadd);
        void setNick(std::string nick);
        void setChannel(Channel* channel);
        void setPasswd(std::string passwd);
        void setInputBuffer(std::string& buffer);
        void removeInvitation(std::string channel);
        void removeChannel(Channel *channel);
        void CleanChannels();
};

#endif


