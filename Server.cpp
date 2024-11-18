#include "Server.hpp"

Server::~Server()
{
    //std::cout << "Server destructor called" << std::endl;
}

Server::Server() : Port(0), SerSocketFd(-1)
{
    //std::cout << "Server constructor called" << std::endl;
    std::cout << "Default Server constructor called" << std::endl;
}

Server::Server(int port, std::string passwd) : Port(port), SerSocketFd(-1), Passwd(passwd)
{
    //std::cout << "Server constructor called" << std::endl;
}

Server& Server::operator=(const Server &other)
{
	//std::cout << "Server copy assignment operator called" << std::endl;
	this->fds = other.fds;
	this->Port = other.Port;
	this->Signal = other.Signal;
	this->Passwd = other.Passwd;
	this->clients = other.clients;
	this->SerSocketFd = other.SerSocketFd;
	return *this;
}

Server::Server(const Server &cp)
{
	//std::cout << "Server copy constructor called" << std::endl;
	*this = cp;
}

void Server::CloseFds()
{
    for (size_t i = 0; i < clients.size(); i++) {
        int clientFd = clients[i].getFd();
        if (close(clientFd) == -1)
            std::cerr << RED << "Error closing client descriptor <" << clientFd << ">: " << std::endl;
        else
            std::cout << RED << "Client <" << clientFd << "> Disconnected" << WHI << std::endl;
    }
    clients.clear();

    if (SerSocketFd != -1) {
        if (close(SerSocketFd) == -1)
           std::cerr << RED << "Error closing server descriptor <" << SerSocketFd << ">: " << std::endl;
        else
            std::cout << RED << "Server <" << SerSocketFd << "> Disconnected" << WHI << std::endl;
        SerSocketFd = -1;
    }
    fds.clear();
}

void Server::ClearClient(int fd)
{
	for(size_t i = 0; i < fds.size(); i++){
		if (fds[i].fd == fd) {
			fds.erase(fds.begin() + i); 
			break;
		}
	}
	for(size_t i = 0; i < clients.size(); i++){
		if (clients[i].getFd() == fd) {
			clients.erase(clients.begin() + i);
			break;
		}
	}
}

void Server::CmdPASS(size_t i, std::vector<std::string> parametros, char buff[1024])
{
	if (parametros.size() != 2)
		std::cout << "Wrong parameters. " << buff << std::endl;
	else
	{
		if((parametros[1] == this->Passwd))
		{
			if (clients[i].getPasswd() == true)
			{
				std::cout << "Password alredy done. " << buff << std::endl;
				return ;
			}
			clients[i].setPasswd(true);
			std::cout << "Correct password. " << buff << std::endl;
		}
		else
			std::cout << "Wrong password. " << buff << std::endl;
	}
}

void Server::CmdNICK(size_t i, std::vector<std::string> parametros, char buff[1024])
{
	if(clients[i].getPasswd() == false)
		std::cout << "Empty Pass. " << buff << std::endl;
	else if (parametros.size() != 2)
		std::cout << "Wrong parameters. " << buff << std::endl;
	else if((!isdigit(parametros[1][0])) && \
	(parametros[1].find(":") == std::string::npos) && \
	(parametros[1].find(",") == std::string::npos) && \
	(parametros[1].size() <= 9))
	{
		for (size_t j = 0; j < clients.size(); j++) {
            if (j != i && parametros[1] == clients[j].getNick()) {
                std::cout << "Nick is already in use. " << buff << std::endl;
                return;
            }
        }
		clients[i].setNick(parametros[1]);
		std::cout << "Correct Nick. " << buff << std::endl;
	}
	else
		std::cout << "Wrong Nick. " << buff << std::endl;
}

void Server::CmdUSER(size_t i, std::vector<std::string> parametros, char buff[1024])
{
	if(clients[i].getPasswd() == false)
		std::cout << "Empty Pass. " << buff << std::endl;
	else if(clients[i].getNick().empty())
		std::cout << "Empty Nick. " << buff << std::endl;
	else if (parametros.size() < 5)
		std::cout << "Wrong parameters. " << buff << std::endl;
	else if((parametros[1].find(":") == std::string::npos) && \
	(parametros[2].find_first_not_of("0123456789") == std::string::npos) && \
	(parametros[3].find(":") == std::string::npos))
	{
		std::string str(buff);
		size_t i = str.find(":");
		if (i != std::string::npos) 
		{
			if(!clients[i].getUser().empty())
			{	
				std::cout << "User alredy done. " << buff << std::endl;
				return ;
			}
			std::string temp = str.substr(i + 1);
			if (!temp.empty())
				clients[i].setRealN(temp);
			clients[i].setUser(parametros[1]);
			clients[i].setUserM(atoi(parametros[2].c_str()));
			std::cout << "Correct User. " << buff << std::endl;
		} 
		else
			std::cout << "No ':' found in buffer. " << buff << std::endl;
	}
	else
		std::cout << "Wrong User. " << buff << std::endl;
}

void Server::CmdPRIVMSG(size_t i, std::vector<std::string> parametros, char buff[1024])
{
	(void)parametros;
	if(clients[i].getPasswd() == false)
		std::cout << "Empty Pass. " << buff << std::endl;
	else if(clients[i].getNick().empty())
		std::cout << "Empty Nick. " << buff << std::endl;
	else if(clients[i].getUser().empty())
		std::cout << "Empty User. " << buff << std::endl;
	else
		std::cout << YEL << "Client <" << fds[i].fd << "> Data: " << WHI << buff << std::endl;
}

std::vector<std::string> ft_split(const char* str, char delimiter) 
{
    std::vector<std::string> result;
    char* copy = strdup(str);
    char* token = strtok(copy, &delimiter);

    while (token != NULL)
	{
        result.push_back(std::string(token));
        token = strtok(NULL, &delimiter);
    }
    free(copy);
    return result;
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
		if(bytes > 1) //pt salto de linea.
		{
			buff[bytes - 1] = '\0';
			std::vector<std::string> parametros = ft_split(buff, 32);
			if (parametros[0] == "PASS")
				CmdPASS(i, parametros, buff);
			else if (parametros[0] == "NICK")
				CmdNICK(i, parametros, buff);
			else if (parametros[0] == "USER")
				CmdUSER(i, parametros, buff);
			else if (parametros[0] == "PRIVMSG")
				CmdPRIVMSG(i, parametros, buff);
			else
				std::cout << "Command not found. " << buff << std::endl;
		}
	}
}

void Server::AcceptNewClient()
{
	Client cli;
	struct sockaddr_in add;
	struct pollfd NewPoll;
	socklen_t len = sizeof(add);

	int incofd = accept(SerSocketFd, (sockaddr *)&(add), &len);
	if (incofd == -1)
		{std::cout << "accept() failed" << std::endl; return;}
	//Conectarse al servidor
	if (fcntl(incofd, F_SETFL, O_NONBLOCK) == -1) 
		{std::cout << "fcntl() failed" << std::endl; return;}
	/*Configuraciones en Fd: 
		1. F__SETFL: Para Establecer Flags
		2. O_NONBLOCK: Modo no bloqueante */

	NewPoll.fd = incofd;
	NewPoll.events = POLLIN; //POLLIN: Monitorea si hay datos disponibles para leer en el descriptor.
	NewPoll.revents = 0; //No hay evento previo.

	cli.setFd(incofd);
	cli.setIp(inet_ntoa((add.sin_addr)));
	clients.push_back(cli);
	fds.push_back(NewPoll);

	std::cout << GRE << "Client <" << incofd << "> Connected" << WHI << std::endl;
}

/* Chuleta de Structs de Red: 

struct sockaddr_in {
	sa_family_t    sin_family;   
		// Familia de direcciones. (IPv4)
	in_port_t      sin_port;     
		// Puerto, almacenado bytes de red (htons).
	struct in_addr sin_addr;     
		// Otro struct: in_addr
	char	sin_zero[8];  
		// - (no se usa) }; 

struct in_addr {
		in_addr_t s_addr;
		// Dirección IP en formato binario.
	};

struct pollfd;
	  int   fd;        
	  // socket a monitorear.
	  short events;     
	  // Eventos que monitoreamos.
	  short revents;
	  // Eventos que ocurrieron. */

void Server::SocketInit()
{
	int en = 1;
	Client aux;  //nodo vacio para dejar el codigo mas limpio y manejable.
	struct sockaddr_in add;
	struct pollfd NewPoll;

	add.sin_family = AF_INET;  //IPv4
	add.sin_addr.s_addr = INADDR_ANY;  //Todas las interfaces.
	add.sin_port = htons(this->Port);

	this->SerSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	if(this->SerSocketFd == -1)
		throw(std::runtime_error("faild to create socket"));
	/*Create socket.
		- SOCK_STREAM: Flag para el uso de protocolo de TCP. */

	if(setsockopt(this->SerSocketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
		throw(std::runtime_error("faild to set option (SO_REUSEADDR) on socket"));
	/*Establecer configuración.
		- SOL_SOCKET: Opciones estándar, comunes para cualquier tipo de socket.
		- SO_REUSEADDR: Permite reutilizar la dirección local, suele haber un 
		tiempo de espera entre ejecucion y ejecucion puesto que se debe liberar la dirrecion
		pero al usar esta Flag nos libramos de esa espera. */
		
	if (fcntl(this->SerSocketFd, F_SETFL, O_NONBLOCK) == -1) 
		throw(std::runtime_error("faild to set option (O_NONBLOCK) on socket"));
	/*Configuraciones en Fd: 
		1. F__SETFL: Para Establecer Flags
		2. O_NONBLOCK: Modo no bloqueante, lo que pide el subject */
	
	if (bind(this->SerSocketFd, (struct sockaddr *)&add, sizeof(add)) == -1)
		throw(std::runtime_error("faild to bind socket"));
	//Asignar IP y puerto.

	if (listen(this->SerSocketFd, SOMAXCONN) == -1) 
		throw(std::runtime_error("listen() faild"));
	//Socket en modo pasivo, listo para aceptar conexiones.

	NewPoll.fd = this->SerSocketFd;
	NewPoll.events = POLLIN; //POLLIN: Monitorea si hay datos disponibles para leer en el descriptor
	NewPoll.revents = 0; //No hay evento previo.
	fds.push_back(NewPoll);
	clients.push_back(aux);
}

void Server::ServerInit()
{
	SocketInit();
	std::cout << GRE << "Server <" << WHI << this->SerSocketFd << GRE << "> Connected" << WHI << std::endl;
	std::cout << "Port: " << this->Port << std::endl; 
	std::cout << "Passwd: " << this->Passwd << std::endl;
	std::cout << "Waiting to accept a connection...\n";
	while (Server::Signal.getSignal() == false) 
	{
		if((poll(&fds[0],fds.size(),-1) == -1) && Server::Signal.getSignal() == false) //poll(); espera eventos de los sockets
			throw(std::runtime_error("poll() faild"));
		/* poll(); espera eventos de los sockets
			- En este caso estamos pendintes del fds[0], el fd del server para 
			estar pedientes de nuevas conexiones. */
		for (size_t i = 0; i < fds.size(); i++)
		{
			if (fds[i].revents & POLLIN){  //checkeamos que tengamos eventos pendientes
				if (fds[i].fd == SerSocketFd) //checkeamos si es el fd del server
					AcceptNewClient();
				else  //posible error, necesidad de control de errores para un segundo user.
					ReceiveNewData(i);
			}
		}
	}
	CloseFds();
}