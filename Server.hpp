#ifndef  SERVER_HPP
# define  SERVER_HPP

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
#include <cstring>
#include "Client.hpp"
//-------------------------------------------------------//
#define RED "\e[1;31m" //-> for red color
#define WHI "\e[0;37m" //-> for white color
#define GRE "\e[1;32m" //-> for green color
#define YEL "\e[1;33m" //-> for yellow color
//-------------------------------------------------------//

class Server
{
    protected:

    private:
        int Port;
        int SerSocketFd;
        static bool Signal;
        std::vector<Client> clients;
        std::vector<struct pollfd> fds;

    public:
        ~Server(void);
        Server(void);
        Server(const Server &cp);
        Server& operator=(const Server &other);
        void CloseFds(void);
        void ServerInit(void);
        void SocketInit(void);
        void ClearClients(int fd);
        void AcceptNewClient(void);
        void ReceiveNewData(int fd);
        static void SignalHandler(int signum);
};

#endif
