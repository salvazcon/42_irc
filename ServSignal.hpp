#ifndef  SERVSIGNAL_HPP
# define  SERVSIGNAL_HPP

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

class ServSignal
{
    protected:

    private:
        static bool Signal;

    public:
        ~ServSignal(void);
        ServSignal(void);
        ServSignal(const ServSignal &cp);
		ServSignal& operator=(const ServSignal &other);
        static void SignalHandler(int signum);
        void setSignal(bool Signal);
        bool getSignal();
};

#endif
