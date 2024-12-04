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
	int port = atoi(argc[1]);
	if(port < 1024 || port > 65535)
	{
        std::cerr << "Error: Invalid Port." << std::endl;
        return 1;
    }
	Server ser(port, argc[2]);
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