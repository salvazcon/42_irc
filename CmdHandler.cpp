#include "CmdHandler.hpp"

void CmdPASS(std::vector<Client>& clients, std::string Passwd, size_t i, std::vector<std::string> parametros, char buff[1024])
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

void CmdNICK(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024])
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

void CmdUSER(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024])
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
		size_t j = str.find(":");
		if (j != std::string::npos) 
		{
			if(!clients[i].getUser()->getUserName().empty())
			{	
				std::cout << "User alredy done. " << buff << std::endl;
				return ;
			}
			std::string temp = ft_normalize_spaces(str.substr(j + 1));
			User user(parametros[1], temp, atoi(parametros[2].c_str()));
			clients[i].setUser(user);
			std::cout << "Correct User. " << buff << std::endl;
		}
		else
			std::cout << "No ':' found in buffer. " << buff << std::endl;
	}
	else
		std::cout << "Wrong User. " << buff << std::endl;
}

void CmdJOIN(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024])
{   
    if(clients[i].getPasswd() == false)
		std::cout << "Empty Pass. " << buff << std::endl;
	else if(clients[i].getNick().empty())
		std::cout << "Empty Nick. " << buff << std::endl;
	else if(clients[i].getUser()->getUserName().empty())
		std::cout << "Empty User. " << buff << std::endl;
    else if (parametros.size() < 2 && parametros.size() > 3)
		std::cout << "Wrong parameters. " << buff << std::endl;
    std::vector<std::string> chanels = ft_split(parametros[1].c_str(), ',');
    if(parametros.size() > 2)
        std::vector<std::string> passwds = ft_split(parametros[2].c_str(), ',');
    /* for(size_t i = 0; i < chanels.size(); i++)
    {
        if (chanels[0] != "&" && parametros[0] != "#")
        {
            std::cout << "Invalid channel. " << buff << std::endl;
            return ;
        }
		for (size_t j = 0; j < clients.size(); j++)
		{
			for (size_t k = 0; j < clients[i].getChannels().size(); k++)
			{
				if (k != j && parametros[1] == clients[i].getChannel(k)->getName()) {
					std::cout << "Channel is already in use. " << buff << std::endl;
					// se unira al canal, si tiene le passwd.
					return;
				}
				else
					std::cout << "New Channel created. " << buff << std::endl;	
					//crearar el channel nuevo.
        	}
		}
		//Falta por parsear si hay que evitra algun caracter especial.
		//  Interaccion con canales:
			//- Busca si existe el canal.
			//- Si no existe lo crea, en caso contrario se une.

    } */
}

void CmdPRIVMSG(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024])
{
	(void)parametros;
	if(clients[i].getPasswd() == false)
		std::cout << "Empty Pass. " << buff << std::endl;
	else if(clients[i].getNick().empty())
		std::cout << "Empty Nick. " << buff << std::endl;
	else if(clients[i].getUser()->getUserName().empty())
		std::cout << "Empty User. " << buff << std::endl;
	else
	{
		//std::cout << YEL << "Client Data: " << WHI << buff << std::endl;
		std::cout << "PASS: " << clients[i].getPasswd() << "." << std::endl;
		std::cout << "NICK: " << clients[i].getNick() << "." << std::endl;
		std::cout << "USER: " << std::endl;
		std::cout << "- username: " << clients[i].getUser()->getUserName() << "." << std::endl;
		std::cout << "- realname: " << clients[i].getUser()->getRealName() << "." << std::endl;
		std::cout << "- usermode: " << clients[i].getUser()->getUserMode() << "." << std::endl;
	}
}
