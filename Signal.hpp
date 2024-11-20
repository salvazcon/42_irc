#ifndef  SIGNAL_HPP
# define  SIGNAL_HPP

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

class Signal
{
    protected:

    private:
        static bool signal;

    public:
        ~Signal(void);
        Signal(void);
        Signal(const Signal &cp);
		Signal& operator=(const Signal &other);
        static void SignalHandler(int signum);
        void setSignal(bool Signal);
        bool getSignal();
};

#endif
