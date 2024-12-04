#include "CmdHandler.hpp"

/* 
- Crear mensaje para usuario y server de forma consecutiva.
*/

int ForbiddenSpace(std::string word)
{
	if((word.find("\t") == std::string::npos) && \
	(word.find("\n") == std::string::npos) && \
	(word.find("\r") == std::string::npos))
		return 0;
	return 1;
}

int ForbiddenJoin(std::string word)
{
	if((word.find(":") == std::string::npos) && \
	(word.find("-") == std::string::npos) && \
	(word.find("+") == std::string::npos) && \
	(word.find("@") == std::string::npos) && \
	(word.find("!") == std::string::npos) && \
	(word.find("~") == std::string::npos) && \
	(!ForbiddenSpace(word)))
		return 0;
	return 1;
}

int ForbiddenChars(std::string word)
{
	if((word.find("#") == std::string::npos) && \
	(word.find("&") == std::string::npos) && \
	(word.find(",") == std::string::npos) && \
	(!ForbiddenJoin(word)))
		return 0;
	return 1;
}

void ListenEverybody(std::vector<Client>& clients, size_t i, size_t j, std::string msg, size_t x)
{
	for(size_t k = 0; k < clients[i].getChannel(j)->getUsers().size(); k++)
		for(size_t m = 0; m < clients.size(); m++)
			if((m != x) && clients[m].getNick() == clients[i].getChannel(j)->getUser(k))
				send(clients[m].getFd(), msg.c_str(), strlen(msg.c_str()), 0);
}

void SendMsg(std::vector<Client>& clients, size_t i, std::string msg)
{
	send(clients[i].getFd(), msg.c_str(), strlen(msg.c_str()), 0);
}

void CmdPASS(std::vector<Client>& clients, std::string Passwd, size_t i, std::vector<std::string> parametros, char buff[1024])
{
	if (parametros.size() < 2)
		std::cout << "Client <" << clients[i].getFd() << "> "<< buff << " :Not enough parameters" << std::endl;
	else if (parametros.size() != 2)
		std::cout << "Client <" << clients[i].getFd() << "> " << buff << " :Erroneus parameters" << std::endl;
	else
	{
		if((parametros[1] == Passwd))
		{
			if (clients[i].getPasswd() == true)
			{
				std::cout << "Client <" << clients[i].getFd() << "> :You may not reregister" << std::endl;
				return ;
			}
			clients[i].setPasswd(true);
			std::cout << "Client <" << clients[i].getFd() << "> :Password correct" << std::endl;
		}
		else
			std::cout << "Client <" << clients[i].getFd() << "> :Password incorrect" << std::endl;
	}
}

void CmdNICK(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024])
{
	if(clients[i].getPasswd() == false)
		std::cout << "Client <" << clients[i].getFd() << "> :You have not registered" << std::endl;
	else if (parametros.size() == 1)
		std::cout << "Client <" << clients[i].getFd() << "> :No nickname given" << std::endl;
	else if (parametros.size() != 2)
		std::cout << "Client <" << clients[i].getFd() << "> " << buff << " :Erroneus parameters" << std::endl;
	else if((parametros[1].size() <= 9) && !ForbiddenChars(parametros[1]))
	{
		for (size_t j = 0; j < clients.size(); j++)
		{
            if (j != i && parametros[1] == clients[j].getNick())
			{
                std::cout << "Client <" << clients[i].getFd() << "> "<< parametros[1] << " :Nickname is already in use" << std::endl;
                return ;
            }
        }
		if(!clients[i].getNick().empty())
		{
			std::cout << "Client <" << clients[i].getFd() << "> :" << clients[i].getNick() << " NICK " << parametros[1] << std::endl;
			for (size_t j = 0; j < clients[i].getChannels().size(); j++)
			{
				for(size_t k = 0; k < clients[i].getChannel(j)->getUsers().size(); k++)
				{
					if (clients[i].getNick() == clients[i].getChannel(j)->getUser(k))
					{
						clients[i].getChannel(j)->getUsers().erase(clients[i].getChannel(j)->getUsers().begin() + k);
						clients[i].getChannel(j)->setUser(parametros[1]);
					}
				}
			}
		}
		else
			std::cout << "Client <" << clients[i].getFd() << "> :Nick correct" << std::endl;
		clients[i].setNick(parametros[1]);
	}
	else
		std::cout << "Client <" << clients[i].getFd() << "> :Erroneus nickname" << std::endl;
}

void CmdUSER(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024])
{
	if((clients[i].getPasswd() == false ) || (clients[i].getNick().empty()))
		std::cout << "Client <" << clients[i].getFd() << "> :You have not registered" << std::endl;
	else if (parametros.size() < 5)
		std::cout << "Client <" << clients[i].getFd() << "> "<< buff << " :Not enough parameters" << std::endl;
	else if ((!ForbiddenChars(parametros[1])) && \
	(parametros[2] == "0") && \
	(parametros[3] == "*") && \
	(parametros[4][0] == ':') && \
	(!ForbiddenSpace(parametros[4])) && \
	((parametros[4].size() > 1) || ((parametros.size() > 5) && (!ForbiddenSpace(parametros[5])))))
	{
		std::string str(buff);
		size_t j = str.find(":");
		std::string temp = ft_normalize_spaces(str.substr(j + 1));
		if(!clients[i].getUser()->getUserName().empty())
		{	
			std::cout << "Client <" << clients[i].getFd() << "> :You may not reregister" << std::endl;
			return ;
		}
		User user(parametros[1], temp, atoi(parametros[2].c_str()));
		clients[i].setUser(user);
		std::cout << "Client <" << clients[i].getFd() << "> :User correct" << std::endl;
	}
	else
		std::cout << "Client <" << clients[i].getFd() << "> :Erroneus username" << std::endl;
}

int	SearchChannel(std::vector<Client>& clients, size_t i, std::vector<std::string> temp_channels, std::vector<std::string> temp_passwds, size_t j)
{
	for (size_t k = 0; k < clients.size(); k++)
	{
		for (size_t l = 0; l < clients[k].getChannels().size(); l++)
		{
			if (temp_channels[j] == clients[k].getChannel(l)->getName()) 
			{
				for(size_t m = 0; m < clients[k].getChannel(l)->getUsers().size(); m++)
					if(clients[i].getNick() == clients[k].getChannel(l)->getUser(m))
						return 1;
				if(!clients[k].getChannel(l)->getPasswd().empty())
				{
					if (temp_passwds.size() > j && clients[k].getChannel(l)->getPasswd() == temp_passwds[j])
					{	
						clients[k].getChannel(l)->setUser(clients[i].getNick());
						clients[i].setChannel(clients[k].getChannel(l));
						std::cout << "Client <" << clients[i].getFd() << "> " << temp_channels[j] << " :Join channel" << std::endl;
						ListenEverybody(clients, k, l, clients[i].getNick() + " JOIN " + temp_channels[j] + "\n", 0);
					}
					else
						std::cout << "Client <" << clients[i].getFd() << "> " << temp_channels[j] << " :Cannot join channel (+k)" << std::endl;
				}
				else
				{
					clients[k].getChannel(l)->setUser(clients[i].getNick());
					clients[i].setChannel(clients[k].getChannel(l));
					std::cout << "Client <" << clients[i].getFd() << "> " << temp_channels[j] << " :Join channel" << std::endl;
					ListenEverybody(clients, k, l, clients[i].getNick() + " JOIN " + temp_channels[j] + "\n", 0);
				}
				return 1;
			}
		}
	}  
	return 0;
}

void CmdJOIN(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024])
{   
    if((clients[i].getPasswd() == false ) ||(clients[i].getNick().empty()) || (clients[i].getUser()->getUserName().empty()))
		std::cout << "Client <" << clients[i].getFd() << "> :You have not registered" << std::endl;
    else if (parametros.size() < 2)
		std::cout << "Client <" << clients[i].getFd() << "> "<< buff << " :Not enough parameters" << std::endl;
	else if (parametros.size() >= 2 && parametros.size() <= 3)
	{
		if(parametros[1] == "0")
		{
			for(size_t j = 0; j < clients[i].getChannels().size(); j++)
				ListenEverybody(clients, i, j, clients[i].getNick() + " PART " + clients[i].getChannel(j)->getName() + "\n", i);
			clients[i].CleanChannels();
			return ;
		}
		std::vector<std::string> temp_channels = ft_split(parametros[1].c_str(), ',');
		std::vector<std::string> temp_passwds;
		if(parametros.size() > 2)
			temp_passwds = ft_split(parametros[2].c_str(), ',');
		for(size_t j = 0; j < temp_channels.size(); j++)
		{
			if ((temp_channels[j][0] == '&' || temp_channels[j][0] == '#') && (!ForbiddenJoin(temp_channels[j])))
			{
				if(!SearchChannel(clients, i, temp_channels, temp_passwds, j))
				{
					Channel* channel = new Channel(temp_channels[j]);
					if(temp_passwds.size() > j)
						channel->setPasswd(temp_passwds[j]);
					channel->setUser(clients[i].getNick());
					clients[i].setChannel(channel);
					std::cout << "Client <" << clients[i].getFd() << "> " << temp_channels[j] << " :Channel created" << std::endl;
					std::string msg = clients[i].getNick() + " JOIN " + temp_channels[j] + "\n";
					send(clients[i].getFd(), msg.c_str(), strlen(msg.c_str()), 0);
				}
			}
			else
				std::cout << "Client <" << clients[i].getFd() << "> " << temp_channels[j] << " :Bad Channel Mask" << std::endl;
		}
	}
	else
		std::cout << "Client <" << clients[i].getFd() << "> " << buff << " :Erroneus parameters" << std::endl;
}

int SearchTarget(std::vector<Client>& clients, std::string target, std::string msg, size_t i)
{
	if (target[0] == '&' || target[0] == '#')
	{
		for (size_t j = 0; j < clients[i].getChannels().size(); j++)
		{
			if (target == clients[i].getChannel(j)->getName())
			{
				ListenEverybody(clients, i, j, msg, 0);
				return 1;
			}
		}
	}
	else
	{
		for(size_t j = 0; j < clients.size(); j++)
		{
			if(target == clients[j].getNick())
			{
				send(clients[j].getFd(), msg.c_str(), strlen(msg.c_str()), 0);
				return 1;
			}
		}
	}
	return 0;
}

void CmdPRIVMSG(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024])
{
	if((clients[i].getPasswd() == false ) ||(clients[i].getNick().empty()) || (clients[i].getUser()->getUserName().empty()))
		std::cout << "Client <" << clients[i].getFd() << "> :You have not registered" << std::endl;
	else if (parametros.size() < 2)
		std::cout << "Client <" << clients[i].getFd() << "> "<< buff << " :Not enough parameters" << std::endl;
	else if ((parametros[2][0] == ':') && (parametros[2].size() > 1 || parametros.size() > 3))
	{
		std::vector<std::string> temp_target = ft_split(parametros[1].c_str(), ',');
		std::string str(buff);
		size_t n = str.find(":");
		std::string temp_msg = str.substr(n + 1);
		if(temp_msg.empty())
		{
			std::cout << "Client <" << clients[i].getFd() << "> "<< buff << " :No text to send" << std::endl;
			return ;
		}
		temp_msg = clients[i].getNick() + ":" + temp_msg + "\n";
		for(size_t j = 0; j < temp_target.size(); j++)
			if(!SearchTarget(clients, temp_target[j], temp_msg, i))
				std::cout << "Client <" << clients[i].getFd() << "> " << temp_target[j] << " :No such nick/channel" << std::endl;
	}
	else
		std::cout << "Client <" << clients[i].getFd() << "> " << buff << " :Erroneus parameters" << std::endl;
}

void CmdINFO(std::vector<Client>& clients, size_t i, char buff[1024])
{
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
