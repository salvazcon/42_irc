#include "Server.hpp"
#include "Client.hpp"

Server::~Server()
{
    //std::cout << "Server destructor called" << std::endl;
}

Server::Server(): SerSocketFd(-1)
{
    //std::cout << "Server constructor called" << std::endl;
}

/* Server::Server(std::string port, std::string pswd): SerSocketFd(-1)
{
    this->port = stoi(port);
	this->port = stoi(pswd);
	std::cout << "Server constructor called" << std::endl;
} */

Server& Server::operator=(const Server &other)
{
    //(<-- Datos a copiar.)
	(void)other;
	//std::cout << "Server copy assignment operator called" << std::endl;
	return *this;
}

Server::Server(const Server &cp)
{
	//std::cout << "Server copy constructor called" << std::endl;
	*this = cp;
}

bool Server::Signal = false; //-> initialize the static boolean
void Server::SignalHandler(int signum)
{
	(void)signum;
	std::cout << std::endl << "Signal Received!" << std::endl;
	Server::Signal = true;
}

void Server::CloseFds()
{
	for(size_t i = 0; i < clients.size(); i++){
		std::cout << RED << "Client <" << clients[i].GetFd() << "> Disconnected" << WHI << std::endl;
		close(clients[i].GetFd());
	}
	if (SerSocketFd != -1){
		std::cout << RED << "Server <" << SerSocketFd << "> Disconnected" << WHI << std::endl;
		close(SerSocketFd);
	}
}

void Server::ClearClient(int fd)
{
	for(size_t i = 0; i < fds.size(); i++){
		if (fds[i].fd == fd) {
			fds.erase(fds.begin() + i); break;
		}
	}
	for(size_t i = 0; i < clients.size(); i++){
		if (clients[i].GetFd() == fd) {
			clients.erase(clients.begin() + i); break;
		}
	}
}

void Server::ReceiveNewData(int fd)
{
	char buff[1024];
	memset(buff, 0, sizeof(buff));
	ssize_t bytes = recv(fd, buff, sizeof(buff) - 1 , 0);

	if(bytes <= 0){
		std::cout << RED << "Client <" << fd << "> Disconnected" << WHI << std::endl;
		ClearClient(fd);
		close(fd);
	} else {
		buff[bytes] = '\0';
		std::cout << YEL << "Client <" << fd << "> Data: " << WHI << buff;
	}
}

void Server::AcceptNewClient() //El uso del cliente no lo tengo muy claro aun, asi que esta inacabado.
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

	cli.SetFd(incofd);
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
		- SO_REUSEADDR: Permite reutilizar la dirección local. */
	
	if (fcntl(this->SerSocketFd, F_SETFL, O_NONBLOCK) == -1) 
		throw(std::runtime_error("faild to set option (O_NONBLOCK) on socket"));
	/*Configuraciones en Fd: 
		1. F__SETFL: Para Establecer Flags
		2. O_NONBLOCK: Modo no bloqueante */
	
	if (bind(this->SerSocketFd, (struct sockaddr *)&add, sizeof(add)) == -1)
		throw(std::runtime_error("faild to bind socket"));
	//Asignar IP y puerto.

	if (listen(this->SerSocketFd, SOMAXCONN) == -1) 
		throw(std::runtime_error("listen() faild"));
	//Socket en modo pasivo, aceptar conexiones.

	NewPoll.fd = this->SerSocketFd;
	NewPoll.events = POLLIN; //POLLIN: Monitorea si hay datos disponibles para leer en el descriptor
	NewPoll.revents = 0; //No hay evento previo.
	fds.push_back(NewPoll);
}

void Server::ServerInit()
{
	this->Port = 4444;
	SocketInit();
	std::cout << GRE << "Server <" << WHI << this->SerSocketFd << GRE << "> Connected" << WHI << std::endl;
	std::cout << "Waiting to accept a connection...\n";
	while (Server::Signal == false) 
	{
		if((poll(&fds[0],fds.size(),-1) == -1) && Server::Signal == false) //poll(); espera eventos de los sockets
			throw(std::runtime_error("poll() faild"));
		for (size_t i = 0; i < fds.size(); i++)
		{
			if (fds[i].revents & POLLIN){
				if (fds[i].fd == SerSocketFd)
					AcceptNewClient();
				else
					ReceiveNewData(fds[i].fd);
			}
		}
	}
	CloseFds();
}