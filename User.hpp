#ifndef  USER_HPP
# define  USER_HPP

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

class User
{
    protected:

    private:
        std::string userName;
        std::string realName;
        int userMode;
    
    public:
        ~User(void);
        User(void);
        User(std::string user, std::string real, int mode);
        User(const User &cp);
		User& operator=(const User &other);
        std::string getUserName();
        std::string getRealName();
        int getUserMode();
        void setUserName(std::string user);
        void setRealName(std::string real);
        void setUserMode(int n);
};

#endif
