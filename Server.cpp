#include "Server.hpp"

Server::~Server()
{
    //std::cout << "Server destructor called" << std::endl;
}

Server::Server() : signal(), serSocketFd(-1)  //Control de errores puerto (MUST DO)
{
    //std::cout << "Server constructor called" << std::endl;
}

Server::Server(int port, std::string passwd) : port(port), signal(), serSocketFd(-1), passwd(passwd)
{
    //std::cout << "Server constructor called" << std::endl;
}

Server& Server::operator=(const Server &other)
{
	//std::cout << "Server copy assignment operator called" << std::endl;
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
	//std::cout << "Server copy constructor called" << std::endl;
	*this = cp;
}

void Server::CloseFds()
{
    for (size_t i = 1; i < clients.size(); i++) {
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
			if (parametros[0] == "PASS")
				CmdPASS(clients, passwd, i, parametros, buff);
			else if (parametros[0] == "NICK")
				CmdNICK(clients, i, parametros, buff);
			else if (parametros[0] == "USER")
				CmdUSER(clients, i, parametros, buff);
			else if(parametros[0] == "JOIN")
				CmdJOIN(clients, i, parametros, buff);
			else if (parametros[0] == "PRIVMSG")
				CmdPRIVMSG(clients, i, parametros, buff);
			else
				std::cout << "Command not found. " << buff << std::endl;
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
	//Conectarse al servidor
	if (fcntl(incofd, F_SETFL, O_NONBLOCK) == -1) 
		{std::cout << "fcntl() failed" << std::endl; return;}
	/*Configuraciones en Fd: 
		1. F__SETFL: Para Establecer Flags
		2. O_NONBLOCK: Modo no bloqueante */

	newPoll.fd = incofd;
	newPoll.events = POLLIN; //POLLIN: Monitorea si hay datos disponibles para leer en el descriptor.
	newPoll.revents = 0; //No hay evento previo.

	cli.setFd(incofd);
	cli.setIp(inet_ntoa((add.sin_addr)));
	clients.push_back(cli);
	fds.push_back(newPoll);

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
	struct pollfd newPoll;

	add.sin_family = AF_INET;  //IPv4
	add.sin_addr.s_addr = INADDR_ANY;  //Todas las interfaces.
	add.sin_port = htons(this->port);

	this->serSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	if(this->serSocketFd == -1)
		throw(std::runtime_error("faild to create socket"));
	/*Create socket.
		- SOCK_STREAM: Flag para el uso de protocolo de TCP. */

	if(setsockopt(this->serSocketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
		throw(std::runtime_error("faild to set option (SO_REUSEADDR) on socket"));
	/*Establecer configuración.
		- SOL_SOCKET: Opciones estándar, comunes para cualquier tipo de socket.
		- SO_REUSEADDR: Permite reutilizar la dirección local, suele haber un 
		tiempo de espera entre ejecucion y ejecucion puesto que se debe liberar la dirrecion
		pero al usar esta Flag nos libramos de esa espera. */
		
	if (fcntl(this->serSocketFd, F_SETFL, O_NONBLOCK) == -1) 
		throw(std::runtime_error("faild to set option (O_NONBLOCK) on socket"));
	/*Configuraciones en Fd: 
		1. F__SETFL: Para Establecer Flags
		2. O_NONBLOCK: Modo no bloqueante, lo que pide el subject */
	
	if (bind(this->serSocketFd, (struct sockaddr *)&add, sizeof(add)) == -1)
		throw(std::runtime_error("faild to bind socket"));
	//Asignar IP y puerto.

	if (listen(this->serSocketFd, SOMAXCONN) == -1) 
		throw(std::runtime_error("listen() faild"));
	//Socket en modo pasivo, listo para aceptar conexiones.

	newPoll.fd = this->serSocketFd;
	newPoll.events = POLLIN; //POLLIN: Monitorea si hay datos disponibles para leer en el descriptor
	newPoll.revents = 0; //No hay evento previo.
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
		if((poll(&fds[0],fds.size(),-1) == -1) && signal.getSignal() == false) //poll(); espera eventos de los sockets
			throw(std::runtime_error("poll() faild"));
		/* poll(); espera eventos de los sockets
			- En este caso estamos pendintes del fds[0], el fd del server para 
			estar pedientes de nuevas conexiones. */
		for (size_t i = 0; i < fds.size(); i++)
		{
			if (fds[i].revents & POLLIN){  //checkeamos que tengamos eventos pendientes
				if (fds[i].fd == serSocketFd) //checkeamos si es el fd del server
					AcceptNewClient();
				else  //posible error, necesidad de control de errores para un segundo user.
					ReceiveNewData(i);
			}
		}
	}
	CloseFds();
}