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
#include "Channel.hpp"
#include "Utils.hpp"

    void CmdPASS(std::vector<Client>& clients, std::string Passwd, size_t i, std::vector<std::string> parametros, char buff[1024]);
    void CmdNICK(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024]);
    void CmdUSER(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024]);
    void CmdJOIN(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024]);
    int SearchChannel(std::vector<Client>& clients, int i, std::vector<std::string> temp_channels, std::vector<std::string> temp_passwds, int j, char buff[1024]);
    int SearchUser(std::vector<Client>& clients, size_t i, size_t l, size_t k);
    void CmdPRIVMSG(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024]);

#endif
