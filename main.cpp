#include "Client.hpp"
#include "Server.hpp"

int main(int argv, char *argc[]) {
	if(argv != 3)
    {
        std::cerr << "Error: Invalid Arguments." << std::endl;
        return 1; 
    }
	(void)argc;
	//Server ser(argc[1], argc[2]);
	Server ser;
	std::cout << "---- SERVER ----" << std::endl;
	try {
		signal(SIGINT, Server::SignalHandler);
		signal(SIGQUIT, Server::SignalHandler);
		ser.ServerInit();
	} catch(const std::exception& e) {
		ser.CloseFds();
		std::cerr << e.what() << std::endl;
	}
	std::cout << "The Server Closed!" << std::endl;
}

/*Para usar el chat IRC debes:

 1. ejecutar el programa en un terminal: "./ircserv 4444 mypassword"

 2. abrir un segundo terminal y ejecutar : "nc localhost 4444"
	- Esto abrirá una conexión TCP al puerto 4444 en tu servidor local.
	Desde aquí puedes enviar mensajes que el servidor recibirá e imprimirá
	en la consola. */