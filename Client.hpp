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
        /*static*/ bool Passwd;
        std::string Nickname;
        std::string Username;
        std::string Realname;
        std::string Ip;
        int Usermode;
        int Fd;

    public:
        ~Client(void);
        Client(void);
        Client(const Client &cp);
        Client& operator=(const Client &other);
        int getFd();
        int getUserM();
        bool getPasswd();
        std::string getRealN();
        std::string getIp();
        std::string getUser();
        std::string getNick();
        void setFd(int fd);
        void setIp(std::string ipadd);
        void setUser(std::string user);
        void setNick(std::string nick);
        void setRealN(std::string real);
        void setPasswd(bool passwd);
        void setUserM(int n);
};
#endif
