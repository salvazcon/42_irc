#include "CmdHandler.hpp"

CmdHandler::~CmdHandler()
{
    //std::cout << "CmdHandler destructor called" << std::endl;
}

CmdHandler::CmdHandler()
{
    //std::cout << "CmdHandler constructor called" << std::endl;
}

CmdHandler& CmdHandler::operator=(const CmdHandler &other)
{
	//std::cout << "CmdHandler copy assignment operator called" << std::endl;
    (void)other;
	return *this;
}

CmdHandler::CmdHandler(const CmdHandler &cp)
{
	//std::cout << "CmdHandler copy constructor called" << std::endl;
	*this = cp;
}

void CmdHandler::CmdPASS(std::vector<Client>& clients, std::string Passwd, size_t i, std::vector<std::string> parametros, char buff[1024])
{
	if (parametros.size() != 2)
		std::cout << "Wrong parameters. " << buff << std::endl;
	else
	{
		if((parametros[1] == Passwd))
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

void CmdHandler::CmdNICK(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024])
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

void CmdHandler::CmdUSER(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024])
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

void CmdHandler::CmdPRIVMSG(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024])
{
	(void)parametros;
	if(clients[i].getPasswd() == false)
		std::cout << "Empty Pass. " << buff << std::endl;
	else if(clients[i].getNick().empty())
		std::cout << "Empty Nick. " << buff << std::endl;
	else if(clients[i].getUser().empty())
		std::cout << "Empty User. " << buff << std::endl;
	else
		std::cout << YEL << "Client Data: " << WHI << buff << std::endl;
}
