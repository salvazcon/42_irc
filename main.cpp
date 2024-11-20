#include "Client.hpp"
#include "Server.hpp"
#include "Signal.hpp"

int main(int argv, char *argc[])
{
	if(argv != 3)
    {
        std::cerr << "Error: Invalid Arguments." << std::endl;
        return 1; 
    }
	Server ser(atoi(argc[1]), argc[2]); //Control de errores puerto (MUST DO)
	std::cout << "---- SERVER ----" << std::endl;
	try {
		signal(SIGINT, Signal::SignalHandler);
		signal(SIGQUIT, Signal::SignalHandler);
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
	en la consola. 
	
3. Una vez dentro inicia el usurio:
	PASS "contrasena"
	NICK nickname
	USER username 0 * :realname
		- username: Nombre de usuario que identifica al cliente en el servidor IRC.
		- hostname (aquí es 0)
		- servername (aquí es *)
		- realname: Es solo informativo del usuarios.

			<-- Lo de abajo no esta implentado todavia. -->
			
	JOIN #nuevo_canal
		- si no esta creado se crea
	PRIVMSG #nuevo_canal : hola grupo.
		- envia mensajes. */