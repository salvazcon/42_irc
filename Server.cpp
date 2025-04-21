#include "Server.hpp"

Server::~Server(){}

Server::Server() : signal(), serSocketFd(-1){}

Server::Server(int port, std::string passwd) : port(port), signal(), serSocketFd(-1), passwd(passwd){}

Server& Server::operator=(const Server &other)
{
	this->port = other.port;
	this->signal = other.signal;
	this->serSocketFd = other.serSocketFd;
	this->passwd = other.passwd;
	this->clients = other.clients;
	this->fds = other.fds;
	return *this;
}

Server::Server(const Server &cp)
{
	*this = cp;
}

void Server::CloseFds()
{
    for (size_t i = 1; i < clients.size(); ++i) {
        int clientFd = clients[i].getFd();
        if (close(clientFd) == -1)
            std::cerr << RED << "Error closing client descriptor <" << clientFd << ">: " << std::endl;
        else
            std::cout << RED << "Client <" << clientFd << "> Disconnected" << WHI << std::endl;
    }
    clients.clear();

    if (serSocketFd != -1) {
        if (close(serSocketFd) == -1)
           std::cerr << RED << "Error closing server descriptor <" << serSocketFd << ">: " << std::endl;
        else
            std::cout << RED << "Server <" << serSocketFd << "> Disconnected" << WHI << std::endl;
        serSocketFd = -1;
    }
    fds.clear();
}

void Server::ClearClient(int fd)
{
	for(size_t i = 0; i < fds.size(); ++i){
		if (fds[i].fd == fd) {
			fds.erase(fds.begin() + i); 
			break;
		}
	}
	for(size_t i = 0; i < clients.size(); ++i){
		if (clients[i].getFd() == fd) {
			clients.erase(clients.begin() + i);
			break;
		}
	}
}

int ForbiddenChar(std::vector<std::string> parametros)
{
	for (size_t i = 0; i < parametros.size(); ++i)
		for (size_t j = 0; j < parametros[i].size(); ++j)
			if (parametros[i][j] == '\t' || parametros[i][j] == '\r' || parametros[i][j] == '\n')
				return 1;
	return 0;
}

void Server::ReceiveNewData(size_t i)
{
	char buff[1024];
	memset(buff, 0, sizeof(buff));
	ssize_t bytes = recv(fds[i].fd, buff, sizeof(buff) - 1 , 0);

	if(bytes <= 0) {
		std::cout << RED << "Client <" << fds[i].fd << "> Disconnected" << WHI << std::endl;
		ClearClient(fds[i].fd);
		close(fds[i].fd);
	} else {
		if(bytes > 1)
		{
			buff[bytes - 1] = '\0';
			std::vector<std::string> parametros = ft_split(buff, ' ');
			if (ForbiddenChar(parametros))
				send(clients[i].getFd(), (std::string(buff) + " :Invalid characters in message.\n").c_str(), 33 + strlen(buff), 0);
			else if (parametros[0] == "PASS")
				CmdPASS(clients, passwd, i, parametros, buff);
			else if (parametros[0] == "NICK")
				CmdNICK(clients, i, parametros, buff);
			else if (parametros[0] == "USER")
				CmdUSER(clients, i, parametros, buff);
			else if(parametros[0] == "JOIN")
				CmdJOIN(clients, i, parametros, buff);
			else if (parametros[0] == "PRIVMSG")
				CmdPRIVMSG(clients, i, parametros, buff);
			else if (parametros[0] == "KICK")
				CmdKICK(clients, i, parametros, buff);
			else if (parametros[0] == "TOPIC")
                CmdTOPIC(clients, i, parametros, buff);
			else if (parametros[0] == "INVITE")
    			CmdINVITE(clients, i, parametros, buff);
			else if (parametros[0] == "MODE")
    			CmdMODE(clients, i, parametros, buff);
			else if (parametros[0] == "INFO")
				CmdINFO(clients, i, buff);
			else
				send(clients[i].getFd(), (std::string(buff) + " :Command not found.\n").c_str(), 21 + strlen(buff), 0);
		}
	}
}

void Server::AcceptNewClient()
{
	Client cli;
	struct sockaddr_in add;
	struct pollfd newPoll;
	socklen_t len = sizeof(add);

	int incofd = accept(serSocketFd, (sockaddr *)&(add), &len);
	if (incofd == -1)
		{std::cout << "accept() failed" << std::endl; return;}
	if (fcntl(incofd, F_SETFL, O_NONBLOCK) == -1) 
		{std::cout << "fcntl() failed" << std::endl; return;}
	newPoll.fd = incofd;
	newPoll.events = POLLIN;
	newPoll.revents = 0;
	cli.setFd(incofd);
	cli.setIp(inet_ntoa((add.sin_addr)));
	clients.push_back(cli);
	fds.push_back(newPoll);
	std::cout << GRE << "Client <" << incofd << "> Connected" << WHI << std::endl;
}

void Server::SocketInit()
{
	int en = 1;
	Client aux;
	struct sockaddr_in add;
	struct pollfd newPoll;

	add.sin_family = AF_INET;
	add.sin_addr.s_addr = INADDR_ANY;
	add.sin_port = htons(this->port);

	this->serSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	if(this->serSocketFd == -1)
		throw(std::runtime_error("faild to create socket"));
	if(setsockopt(this->serSocketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
		throw(std::runtime_error("faild to set option (SO_REUSEADDR) on socket"));		
	if (fcntl(this->serSocketFd, F_SETFL, O_NONBLOCK) == -1) 
		throw(std::runtime_error("faild to set option (O_NONBLOCK) on socket"));	
	if (bind(this->serSocketFd, (struct sockaddr *)&add, sizeof(add)) == -1)
		throw(std::runtime_error("faild to bind socket"));
	if (listen(this->serSocketFd, SOMAXCONN) == -1) 
		throw(std::runtime_error("listen() faild"));
	newPoll.fd = this->serSocketFd;
	newPoll.events = POLLIN;
	newPoll.revents = 0;
	fds.push_back(newPoll);
	clients.push_back(aux);
}

void Server::ServerInit()
{
	SocketInit();
	std::cout << GRE << "Server <" << WHI << this->serSocketFd << GRE << "> Connected" << WHI << std::endl;
	std::cout << "Waiting to accept a connection...\n";
	while (signal.getSignal() == false) 
	{
		if((poll(&fds[0],fds.size(),-1) == -1) && signal.getSignal() == false)
			throw(std::runtime_error("poll() faild"));
		for (size_t i = 0; i < fds.size(); ++i)
		{
			if (fds[i].revents & POLLIN){
				if (fds[i].fd == serSocketFd)
					AcceptNewClient();
				else
					ReceiveNewData(i);
			}
		}
	}
	CloseFds();
}