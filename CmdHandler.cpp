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

int	SearchUser(std::vector<Client>& clients, size_t i, size_t l, size_t k)
{
	for(size_t m = 0; m < clients[k].getChannel(l)->getUsers().size(); m++) // m Users
	{
		if(clients[i].getNick() == clients[k].getChannel(l)->getUser(m))
		{
			std::cout << "User alredy in the channel. " << clients[k].getChannel(l)->getName() << std::endl;
			return 1;
		}
	}
	return 0;
}

int	SearchChannel(std::vector<Client>& clients, size_t i, std::vector<std::string> temp_channels, std::vector<std::string> temp_passwds, size_t j, char buff[1024])
{
	for (size_t k = 0; k < clients.size(); k++) //k clientes
	{
		for (size_t l = 0; l < clients[k].getChannels().size(); l++) //l channels
		{
			if (temp_channels[j] == clients[k].getChannel(l)->getName()) 
			{
				if(clients[k].getChannel(l)->getPasswd().empty())
				{
					if (temp_passwds.size() > j)
						std::cout << "Channel does not require a password. " << temp_passwds[j] << std::endl;
					if(SearchUser(clients, i, l, k))
							return 1;
					clients[k].getChannel(l)->setUser(clients[i].getNick());
					clients[i].setChannel(clients[k].getChannel(l));
					std::cout << "Channel joined. " << clients[k].getChannel(l)->getName() << std::endl;
				}
				else
				{
					if (temp_passwds.size() > j && clients[k].getChannel(l)->getPasswd() == temp_passwds[j])
					{	
						if(SearchUser(clients, i, l, k))
							return 1;
						clients[k].getChannel(l)->setUser(clients[i].getNick());
						clients[i].setChannel(clients[k].getChannel(l));
						std::cout << "Channel joined. " << clients[k].getChannel(l)->getName() << std::endl;
					}
					else
						std::cout << "Invalid channel password. " << buff << std::endl;
				}
				return 1; // no es return, es break.
			}
		}
	}
	return 0;
}

void CmdJOIN(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024])
{   
    if(clients[i].getPasswd() == false)
		std::cout << "Empty Pass. " << buff << std::endl;
	else if(clients[i].getNick().empty())
		std::cout << "Empty Nick. " << buff << std::endl;
	else if(clients[i].getUser()->getUserName().empty())
		std::cout << "Empty User. " << buff << std::endl;
    else if (parametros.size() > 1 && parametros.size() < 4)
	{
		std::vector<std::string> temp_channels = ft_split(parametros[1].c_str(), ',');
		std::vector<std::string> temp_passwds;
		if(parametros.size() > 2)
			temp_passwds = ft_split(parametros[2].c_str(), ',');
		for(size_t j = 0; j < temp_channels.size(); j++)
		{
			if (temp_channels[0][0] == '&' || temp_channels[0][0] == '#')
			{
				if(SearchChannel(clients, i, temp_channels, temp_passwds, j, buff) == 0)
				{
					Channel* channel = new Channel(temp_channels[j]);
					if(temp_passwds.size() > j)
						channel->setPasswd(temp_passwds[j]);
					channel->setUser(clients[i].getNick());
					clients[i].setChannel(channel);
					std::cout << "New Channel created. " << temp_channels[j] << std::endl;
				}
			}
			else
				std::cout << "Invalid channel. " << buff << std::endl;
		}
	}
	else
		std::cout << "Wrong parameters. " << buff << std::endl;
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
		for (size_t k = 1; k < clients.size(); k++)
		{
			//std::cout << YEL << "Client Data: " << WHI << buff << std::endl;
			std::cout << "-----------------------------------------" << std::endl;
			std::cout << "PASS: " << clients[k].getPasswd() << "." << std::endl;
			std::cout << "NICK: " << clients[k].getNick() << "." << std::endl;
			std::cout << "USER: " << std::endl;
			std::cout << "- username: " << clients[k].getUser()->getUserName() << "." << std::endl;
			std::cout << "- realname: " << clients[k].getUser()->getRealName() << "." << std::endl;
			std::cout << "- usermode: " << clients[k].getUser()->getUserMode() << "." << std::endl;
			for (size_t l = 0; l < clients[k].getChannels().size(); l++)
			{
				std::cout << l << "_CHANNEL: " << std::endl;
				std::cout << "- name: " << clients[k].getChannel(l)->getName() << "." << std::endl;
				std::cout << "- passwd: " << clients[k].getChannel(l)->getPasswd() << "." << std::endl;
				std::cout << "- users: ";
				for (size_t n = 0; n < clients[k].getChannel(l)->getUsers().size(); n++){
					std::cout << n << ": " << clients[k].getChannel(l)->getUser(n) << ", ";
				}
				std::cout << std::endl;
			}
			std::cout << "-----------------------------------------" << std::endl;
			std::cout << std::endl;
		}
	}
}
