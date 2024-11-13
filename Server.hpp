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
#include <cstdlib>
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
        std::string Passwd;
        std::vector<Client> clients;
        std::vector<struct pollfd> fds;

    public:
        ~Server(void);
        Server(void);
        Server(int port, std::string passwd);
        Server(const Server &cp);
        Server& operator=(const Server &other);
        void CloseFds();
        void ServerInit();
        void SocketInit();
        void AcceptNewClient();
        void ClearClient(int fd);
        void ReceiveNewData(int fd);
        static void SignalHandler(int signum);
        void CmdUSER(int i, std::vector<std::string> parametros, char buff[1024]);
        void CmdNICK(int i, std::vector<std::string> parametros, char buff[1024]);
        void CmdPASS(int i, std::vector<std::string> parametros, char buff[1024]);
        void CmdPRIVMSG(int i, std::vector<std::string> parametros, char buff[1024]);
};

#endif
