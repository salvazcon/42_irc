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

bool Server::Signal = false; //-> initialize the static boolean

Server::Server(int port, std::string passwd)
{
    this->SerSocketFd = -1;
	this->Port = port;
	this->Passwd = passwd;
	std::cout << "Server constructor called" << std::endl;
}

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

void Server::SignalHandler(int signum)
{
	(void)signum;
	std::cout << std::endl << "Signal Received!" << std::endl;
	Server::Signal = true;
}

void Server::CloseFds()
{
	for(size_t i = 0; i < clients.size(); i++){
		std::cout << RED << "Client <" << clients[i].getFd() << "> Disconnected" << WHI << std::endl;
		close(clients[i].getFd());
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
		if (clients[i].getFd() == fd) {
			clients.erase(clients.begin() + i); break;
		}
	}
}

std::vector<std::string> ft_split(const char* str, char delimiter) 
{
    std::vector<std::string> result;
    char* copy = strdup(str);
    char* token = strtok(copy, &delimiter);

    while (token != NULL) {
        result.push_back(std::string(token));
        token = strtok(NULL, &delimiter);
    }
    free(copy);
    return result;
}

void Server::ReceiveNewData(int i) 
{
	char buff[1024];
	memset(buff, 0, sizeof(buff));
	ssize_t bytes = recv(fds[i].fd, buff, sizeof(buff) - 1 , 0);

	if(bytes <= 0){
		std::cout << RED << "Client <" << fds[i].fd << "> Disconnected" << WHI << std::endl;
		ClearClient(fds[i].fd);
		close(fds[i].fd);
	} else 
	{
		if(bytes > 1) //pt salto de linea.
		{
			buff[bytes - 1] = '\0';
			std::vector<std::string> parametros = ft_split(buff, 32);
			if(clients[i - 1].getPasswd() == false)
			{
				if((parametros[0] == "PASS") && (parametros[1] == this->Passwd))
				{
					clients[i - 1].setPasswd(true);
					return ;
				}
				else
				{
					std::cout << "Contrasena incompleted: " << buff << std::endl;
					return ;
				}
			}
			if(clients[i - 1].getNick().empty())
			{
				if((parametros.size() == 2) && (parametros[0] == "NICK"))
				{
					clients[i - 1].setNick(parametros[1]);
					return ;
				}
				else
				{
					std::cout << "Nick incorrecto: " << buff << std::endl;
					return ;
				}
				/*
				- No puede comenzar con un dígito.
				- Algunos caracteres como espacios, dos puntos (:), y el símbolo de coma (,) están prohibidos.
				- La longitud máxima del apodo suele estar limitada a 9 caracteres en muchas implementaciones de IRC
				*/
			}
			if(clients[i - 1].getUser().empty())
			{
				if((parametros.size() >= 5) && (parametros[0] == "USER"))
				{
					clients[i - 1].setUser(parametros[1]);
					return ;
				}
				else
				{
					std::cout << "User incorrecto: " << buff << std::endl;
					return ;
				}
				/*
				- Algunos caracteres como espacios y dos puntos (:) están prohibidos.
				*/
			}
			else
				std::cout << YEL << "Client <" << fds[i].fd << "> Data: " << WHI << buff << std::endl;
			//Debo hacer un parseo con split.
			//if(Nickname)
			//if(Username)
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
}

void Server::ServerInit()
{
	SocketInit();
	std::cout << GRE << "Server <" << WHI << this->SerSocketFd << GRE << "> Connected" << WHI << std::endl;
	std::cout << "Port: " << this->Port << std::endl; 
	std::cout << "Passwd: " << this->Passwd << std::endl;
	std::cout << "Waiting to accept a connection...\n";
	while (Server::Signal == false) 
	{
		if((poll(&fds[0],fds.size(),-1) == -1) && Server::Signal == false) //poll(); espera eventos de los sockets
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
					//Client[i - 1], fds[i].fd
			}
		}
	}
	CloseFds();
}