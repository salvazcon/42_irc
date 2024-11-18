#ifndef  CMDHANDLER_HPP
# define  CMDHANDLER_HPP

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

class CmdHandler
{
    protected:

    private:
    
    public:
        ~CmdHandler(void);
        CmdHandler(void);
        CmdHandler(const CmdHandler &cp);
		CmdHandler& operator=(const CmdHandler &other);
        void CmdPASS(std::vector<Client>& clients, std::string Passwd, size_t i, std::vector<std::string> parametros, char buff[1024]);
        void CmdNICK(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024]);
        void CmdUSER(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024]);
        void CmdPRIVMSG(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024]);
};

#endif
