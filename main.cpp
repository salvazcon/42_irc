#include "Client.hpp"
#include "Server.hpp"

int main() {
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