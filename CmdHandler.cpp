#include "CmdHandler.hpp"

/*
MUST DO:
  //mensaje error cliente (cliente IRC ejemplo)
  //funcion extra en MODE +-l
*/

int ForbiddenJoin(std::string word)
{
	if((word.find(":") == std::string::npos) && \
	(word.find("-") == std::string::npos) && \
	(word.find("+") == std::string::npos) && \
	(word.find("@") == std::string::npos) && \
	(word.find("!") == std::string::npos) && \
	(word.find("~") == std::string::npos))
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

int DoesExist(std::vector<Client>& clients, std::string target)
{
    if (target[0] == '&' || target[0] == '#')
    {
		for (size_t i = 0; i < clients.size(); ++i)
            for (size_t j = 0; j < clients[i].getChannels().size(); ++j)
                if (clients[i].getChannel(j)->getName() == target)
                    return 1;
    }
	else
        for (size_t i = 0; i < clients.size(); ++i)
            if (clients[i].getNick() == target)
                return 1;
	return 0;
}

void ListenEverybody(std::vector<Client>& clients, Channel *channel, std::string msg, size_t x)
{
    for(size_t k = 0; k < channel->getUsers().size(); ++k)
        for(size_t m = 0; m < clients.size(); ++m)
            if((m != x) && clients[m].getNick() == channel->getUser(k))
                send(clients[m].getFd(), msg.c_str(), strlen(msg.c_str()), 0);
}

int SearchTarget(std::vector<Client>& clients, std::string target, std::string msg, size_t i)
{
	if (target[0] == '&' || target[0] == '#')
	{
		for (size_t j = 0; j < clients[i].getChannels().size(); ++j)
		{
			if (target == clients[i].getChannel(j)->getName())
			{
				ListenEverybody(clients, clients[i].getChannel(j), msg, 0);
				return 1;
			}
		}
	}
	else
	{
		for(size_t j = 0; j < clients.size(); ++j)
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

std::string FtColons(char buff[1024])
{
    std::string msg(buff);
    size_t j = msg.find(":");
    msg = ft_normalize_spaces(msg.substr(j + 1));
    return(msg);
}

void SendMsg(std::vector<Client>& clients, size_t i, std::string msg)
{
	send(clients[i].getFd(), msg.c_str(), strlen(msg.c_str()), 0);
}

void CmdPASS(std::vector<Client>& clients, std::string Passwd, size_t i, std::vector<std::string> parametros, char buff[1024])
{
	if (parametros.size() < 2)
        SendMsg(clients, i, std::string(buff) + " :Not enough parameters\n");
	else if (parametros.size() != 2)
        SendMsg(clients, i, std::string(buff) + " :Erroneus parameters\n");
	else
	{
		if((parametros[1] == Passwd))
		{
			if (clients[i].getPasswd() == true)
			{
				SendMsg(clients, i, "You may not reregister\n");
				return ;
			}
			clients[i].setPasswd(true);
			SendMsg(clients, i, "Password correct\n");
		}
		else
            SendMsg(clients, i, "Password incorrect\n");
	}
}

void CmdNICK(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024])
{
	if(clients[i].getPasswd() == false)
        SendMsg(clients, i, "You have not registered\n");
	else if (parametros.size() == 1)
        SendMsg(clients, i, "No nickname given\n");
	else if (parametros.size() != 2)
		SendMsg(clients, i, std::string(buff) + " :Erroneus parameters\n");
	else if((parametros[1].size() <= 9) && !ForbiddenChars(parametros[1]) && !std::isdigit(parametros[1][0]))
	{
		for (size_t j = 0; j < clients.size(); ++j)
		{
            if (j != i && parametros[1] == clients[j].getNick())
			{
                SendMsg(clients, i, parametros[1] + " :Nickname is already in use\n");
                return ;
            }
        }
		if(!clients[i].getNick().empty())
		{
			SendMsg(clients, i, clients[i].getNick() + " NICK " + parametros[1] + "\n");
			for (size_t j = 0; j < clients[i].getChannels().size(); ++j)
			{
				for(size_t k = 0; k < clients[i].getChannel(j)->getUsers().size(); ++k)
				{
					if (clients[i].getNick() == clients[i].getChannel(j)->getUser(k))
					{
						clients[i].getChannel(j)->getUsers().erase(clients[i].getChannel(j)->getUsers().begin() + k);
						clients[i].getChannel(j)->setUser(parametros[1]);
					}
				}
                ListenEverybody(clients, clients[i].getChannel(j), clients[i].getNick() + " NICK " + parametros[1] + "\n", 0);
			}
		}
		else
            SendMsg(clients, i, "Nick correct\n");
		clients[i].setNick(parametros[1]);
	}
	else
        SendMsg(clients, i, "Erroneus nickname\n");
}

void CmdUSER(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024])
{
	if((clients[i].getPasswd() == false ) || (clients[i].getNick().empty()))
        SendMsg(clients, i, "You have not registered\n");
	else if ((parametros.size() < 5) || (parametros.size() == 5 && parametros[4].size() == 1 && parametros[4][0] == ':'))
        SendMsg(clients, i, std::string(buff) + " :Not enough parameters\n");
	else if ((!ForbiddenChars(parametros[1])) && \
	(parametros[2] == "0") && \
	(parametros[3] == "*") && \
	(parametros[4][0] == ':'))
	{
        std::string realname = FtColons(buff);
		if(!clients[i].getUser()->getUserName().empty())
		{	
			SendMsg(clients, i, "You may not reregister\n");
			return ;
		}
		User user(parametros[1], realname, atoi(parametros[2].c_str()));
		clients[i].setUser(user);
		SendMsg(clients, i, "User correct\n");
        SendMsg(clients, i, "Welcome to the Internet Relay Network " + clients[i].getNick() + "\n");
	}
	else
        SendMsg(clients, i, std::string(buff) + " :Erroneous parameters\n");
}

void ConfigJoin(std::vector<Client>& clients, size_t i, std::vector<std::string> temp_channels, size_t j, size_t k, size_t l)
{
    if(!clients[k].getChannel(l)->getTopic()->getTopic().empty())
        SendMsg(clients, i, temp_channels[j] + " :" + clients[k].getChannel(l)->getTopic()->getTopic() + "\n");
    SendMsg(clients, i, temp_channels[j] + " :");
    for (size_t m = 0; m < clients[k].getChannel(l)->getUsers().size(); ++m)
        SendMsg(clients, i, clients[k].getChannel(l)->getUser(m) + " ");
    SendMsg(clients, i, "\n");
    clients[k].getChannel(l)->setUser(clients[i].getNick());
    clients[i].setChannel(clients[k].getChannel(l));
    SendMsg(clients, i, "JOIN " + temp_channels[j] + "\n");
    ListenEverybody(clients, clients[k].getChannel(l), clients[i].getNick() + " JOIN " + temp_channels[j] + "\n", 0);
    clients[i].removeInvitation(temp_channels[j]);
}

void ChannelMode(std::vector<Client>& clients, size_t i, std::vector<std::string> temp_channels, size_t j, size_t k, size_t l)
{
    if(clients[k].getChannel(l)->getInviteOnly())
    {
        if(clients[i].getInvitation(clients[k].getChannel(l)->getName()))
        {
            if(clients[k].getChannel(l)->getLimitChannel())
            {
                if(clients[k].getChannel(l)->getUsers().size() < clients[k].getChannel(l)->getLimit())   
                    ConfigJoin(clients, i, temp_channels, j, k, l);
                else
                    SendMsg(clients, i, temp_channels[j] + " :Cannot join channel (+l) - Channel is full\n");
            }
            else
                ConfigJoin(clients, i, temp_channels, j, k, l);
        }
        else
            SendMsg(clients, i, temp_channels[j] + " :Cannot join channel (+i)\n");
    }
    else
    {
        if(clients[k].getChannel(l)->getLimitChannel())
        {
            if(clients[k].getChannel(l)->getUsers().size() < clients[k].getChannel(l)->getLimit())   
                ConfigJoin(clients, i, temp_channels, j, k, l);
            else
                SendMsg(clients, i, temp_channels[j] + " :Cannot join channel (+l) - Channel is full\n");
        }
        else
            ConfigJoin(clients, i, temp_channels, j, k, l);
    }
}

int	SearchChannel(std::vector<Client>& clients, size_t i, std::vector<std::string> temp_channels, std::vector<std::string> temp_passwds, size_t j)
{
	for (size_t k = 0; k < clients.size(); ++k)
	{
		for (size_t l = 0; l < clients[k].getChannels().size(); ++l)
		{
			if (temp_channels[j] == clients[k].getChannel(l)->getName())
			{
                for (size_t m = 0; m < clients[k].getChannel(l)->getUsers().size(); ++m)
                    if(clients[i].getNick() == clients[k].getChannel(l)->getUser(m))
                        return 1;
                if (!clients[k].getChannel(l)->getPasswd().empty())
                {
                    if (temp_passwds.size() > j && clients[k].getChannel(l)->getPasswd() == temp_passwds[j])
                        ChannelMode(clients, i, temp_channels, j, k, l);
                    else
                        SendMsg(clients, i, temp_channels[j] + " :Cannot join channel (+k)\n");
                }               
                else
                    ChannelMode(clients, i, temp_channels, j, k, l);
                return 1;
			}
		}
	}  
	return 0;
}

void CmdJOIN(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024])
{   
    if((clients[i].getPasswd() == false ) ||(clients[i].getNick().empty()) || (clients[i].getUser()->getUserName().empty()))
        SendMsg(clients, i, "You have not registered\n");
    else if (parametros.size() < 2)
        SendMsg(clients, i, std::string(buff) + " :Not enough parameters\n");
	else if (parametros.size() >= 2 && parametros.size() <= 3)
	{
		if(parametros[1] == "0")
		{
			for(size_t j = 0; j < clients[i].getChannels().size(); ++j)
				ListenEverybody(clients, clients[i].getChannel(j), clients[i].getNick() + " PART " + clients[i].getChannel(j)->getName() + "\n", i);
			clients[i].CleanChannels();
			return ;
		}
		std::vector<std::string> temp_channels = ft_split(parametros[1].c_str(), ',');
		std::vector<std::string> temp_passwds;
		if(parametros.size() > 2)
			temp_passwds = ft_split(parametros[2].c_str(), ',');
		for(size_t j = 0; j < temp_channels.size(); ++j)
		{
			if ((temp_channels[j][0] == '&' || temp_channels[j][0] == '#') && (!ForbiddenJoin(temp_channels[j])))
			{
                if(!SearchChannel(clients, i, temp_channels, temp_passwds, j))
                {   
                    if(temp_channels[j].length() <= 50)
                    {
                        Channel* channel = new Channel(temp_channels[j]);
                        if(temp_passwds.size() > j)
                            channel->setPasswd(temp_passwds[j]);
                        channel->setOperator(clients[i].getNick());
                        channel->setUser(clients[i].getNick());
                        channel->setFounder(clients[i].getNick());
                        clients[i].setChannel(channel);
                        SendMsg(clients, i, temp_channels[j] + " :Channel created\n");
                        std::string msg = "JOIN " + temp_channels[j] + "\n";
                        send(clients[i].getFd(), msg.c_str(), strlen(msg.c_str()), 0);
                    }
                    else
                        SendMsg(clients, i, temp_channels[j] + " :Invalid channel name (too long)\n");
                }
			}
			else
                SendMsg(clients, i, temp_channels[j] + " :Bad Channel Mask\n");
		}
	}
	else
        SendMsg(clients, i, std::string(buff) + " :Erroneous parameters\n");
}

void CmdPRIVMSG(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024])
{
	if((clients[i].getPasswd() == false ) ||(clients[i].getNick().empty()) || (clients[i].getUser()->getUserName().empty()))
        SendMsg(clients, i, "You have not registered\n");
	else if (parametros.size() < 3)
        SendMsg(clients, i, std::string(buff) + ": Not enough parameters\n");
	else if ((!ForbiddenJoin(parametros[1])) && (parametros[2][0] == ':'))
	{
		std::vector<std::string> temp_target = ft_split(parametros[1].c_str(), ',');
        std::string temp_msg = FtColons(buff);
		if(temp_msg.empty())
			SendMsg(clients, i, std::string(buff) + " : No text to send\n");
        else if (temp_msg.length() > 512)
            SendMsg(clients, i, temp_msg + ": Topic too long\n");
        else
        {
            temp_msg = clients[i].getNick() + ": " + temp_msg + "\n";
            for(size_t j = 0; j < temp_target.size(); ++j)
                if(!SearchTarget(clients, temp_target[j], temp_msg, i))
                    SendMsg(clients, i, temp_target[j] + ": No such nick/channel\n");
        }
	}
	else
        SendMsg(clients, i, std::string(buff) + ": Erroneous parameters\n");
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
        std::cout << "-----------------INFO-------------------" << std::endl;
		for (size_t k = 1; k < clients.size(); ++k)
		{
			std::cout << "-----------------------------------------" << std::endl;
			std::cout << "PASS: " << clients[k].getPasswd() << "." << std::endl;
			std::cout << "NICK: " << clients[k].getNick() << "." << std::endl;
			std::cout << "USER: " << std::endl;
			std::cout << "- username: " << clients[k].getUser()->getUserName() << "." << std::endl;
			std::cout << "- realname: " << clients[k].getUser()->getRealName() << "." << std::endl;
			std::cout << "- usermode: " << clients[k].getUser()->getUserMode() << "." << std::endl;
			for (size_t l = 0; l < clients[k].getChannels().size(); ++l)
			{
				std::cout << l << "_CHANNEL: " << std::endl;
				std::cout << "- name: " << clients[k].getChannel(l)->getName() << "." << std::endl;
				std::cout << "- passwd: " << clients[k].getChannel(l)->getPasswd() << "." << std::endl;
                std::cout << "- founder: " << clients[k].getChannel(l)->getFounder() << "." << std::endl;
				std::cout << "- users: ";
				for (size_t n = 0; n < clients[k].getChannel(l)->getUsers().size(); ++n){
					std::cout << n << ": " << clients[k].getChannel(l)->getUser(n) << ", ";
				}
                std::cout << std::endl;
                std::cout << "- operators: ";
                for (size_t n = 0; n < clients[k].getChannel(l)->getOperators().size(); ++n){
					std::cout << n << ": " << clients[k].getChannel(l)->getOperator(n) << ", ";
				}
				std::cout << std::endl;
			}
			std::cout << "-----------------------------------------" << std::endl;
			std::cout << std::endl;
		}
	}
}

/* Elegimos el methodo simple por falta de informacion sobre ciertos casos en la documnetacion y ejemplos poco claros de chatGPT:
        - se permite el uso de + y - a la vez (hay ejemplos de uso con + y - a la vez), y si es asi el orden es indiferente (...)?
        - el uso de multiples + o - (...)
        - que agrupacion esta permitida? (...)
        - usar el mismo comando a la vez de formas opuestas -i+i (...)
        - usar el mimso comando dos veces seguidas +i+i (ignora la segunda).
        - no hay filtro de agrupacion como se deben de ordenar los parametros de cada opcion (argumentos en el mismo orden que las letras)
        - que pautas de separacion hay, "+i +k -o", "+ik -o", "+ik-o".(...) */

/* Mantenemos la opcion mas simple para evitar situaciones donde no sabemos como explicar lo quie debe hacer y evitar asi contradiciones.
        - si faltan argumentos se ignora el comando (Docu.)
        - si hay mas comandos de la cuenta se indica que es ilegible (Propia eleccion)
*/

void ListModes(std::vector<Client>& clients, size_t i, Channel *channel)
{
    std::string msg;
    if(channel->getTopicProtected())
        msg = msg + "t";
    if(channel->getInviteOnly())
        msg = msg + "i";
    if(channel->getLimitChannel())
        msg = msg + "l";
    if(channel->getPasswd().size() != 0)
        msg = msg + "k " + channel->getPasswd();
    if(msg.size() != 0)
        msg = " +" + msg;
    SendMsg(clients, i, channel->getName() + msg + "\n");
}

int MinusConfig(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, Channel *channel, char buff[1024])
{
    if(parametros[2][1] == 'i')
        channel->setInviteOnly(false);
    else if(parametros[2][1] == 't')
        channel->setTopicProtection(false);
    else if(parametros[2][1] == 'k')
        channel->setPasswd("");
    else if(parametros[2][1] == 'l')
        channel->setLimitChannel(false);
    else if(parametros[2][1] == 'o')
    {
        if (parametros.size() != 4)
        {
            SendMsg(clients, i, std::string(buff) + ": Not enough parameters\n");
            return 0;
        }
        else if (!DoesExist(clients, parametros[3]))
        {
            SendMsg(clients, i, parametros[3] + ": No such nick\n");
            return 0;
        }
        else if (parametros[3] == channel->getFounder())
        {
            SendMsg(clients, i, parametros[3] + ": Cannot remove privileges from channel founder\n");
            return 0;
        }
        else
            channel->removeOperator(parametros[3]);
    }
    else
    {
        SendMsg(clients, i, "Unknown MODE flag\n");
        return 0;
    }
    return 1;
}

int PlusConfig(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, Channel *channel, char buff[1024])
{
    if(parametros[2][1] == 'i')
    {
        channel->setInviteOnly(true);
        return 1;
    }
    else if(parametros[2][1] == 't')
    {
        channel->setTopicProtection(true);
        return 1;
    }
    else if(parametros[2][1] == 'l')
    {
        if (parametros.size() != 4)
            SendMsg(clients, i, std::string(buff) + ": Not enough parameters\n");
        else if (parametros[3].find_first_not_of("0123456789") != std::string::npos || std::atoi(parametros[3].c_str()) < 1 || 2147483647 < std::atoi(parametros[3].c_str()))
            SendMsg(clients, i, parametros[3] + ": Invalid limit\n");
        else
        {
            channel->setLimitChannel(true);
            channel->setLimit(std::atoi(parametros[3].c_str()));
            return 1;
        }
    }
    else if(parametros[2][1] == 'k')
    {
        if (parametros.size() != 4)
            SendMsg(clients, i, std::string(buff) + ": Not enough parameters\n");
        else if (parametros[3].length() > 23)
            SendMsg(clients, i, parametros[3] + ": Password too long\n");
        else
        {
            channel->setPasswd(parametros[3]);
            return 1;
        }
    }
    else if(parametros[2][1] == 'o')
    {
        if (parametros.size() != 4)
            SendMsg(clients, i, std::string(buff) + ": Not enough parameters\n");
        else if(!DoesExist(clients, parametros[3]))
            SendMsg(clients, i, parametros[3] + ": No such nick\n");
        else
        {
            for (size_t j = 0; j < channel->getUsers().size(); ++j)
            {
                if(channel->getUser(j) == parametros[3])
                {
                    channel->setOperator(parametros[3]);
                    return 1;
                }
    
            }
            SendMsg(clients, i, parametros[3] + ": You're not on that channel\n");
        }
    }
    else
        SendMsg(clients, i, "Unknown MODE flag\n");
    return 0;
}

void CmdMODE(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024])
{
    if (clients[i].getPasswd() == false || clients[i].getNick().empty() || clients[i].getUser()->getUserName().empty())
        SendMsg(clients, i, "You have not registered\n");
    else if (parametros.size() < 2)
        SendMsg(clients, i, std::string(buff) + ": Not enough parameters\n");
    else if (parametros.size() >= 2 && parametros.size() <= 4)
    {
        if (parametros[1][0] == '&' || parametros[1][0] == '#')
        {
            if(DoesExist(clients, parametros[1]))
            {
                for (size_t j = 0; j < clients[i].getChannels().size(); ++j)
                {
                    if (clients[i].getChannel(j)->getName() == parametros[1])
                    {
                        for (size_t k = 0; k < clients[i].getChannel(j)->getOperators().size(); ++k)
                        {
                            if (clients[i].getChannel(j)->getOperator(k) == clients[i].getNick())
                            {
                                if (parametros.size() == 2)
                                    ListModes(clients, i, clients[i].getChannel(j));
                                else if(parametros[2].size() == 2 && parametros[2][0] == '-')
                                {
                                    if (MinusConfig(clients, i, parametros, clients[i].getChannel(j), buff))
                                        ListenEverybody(clients, clients[i].getChannel(j), std::string(buff) + "\n", 0);
                                }
                                else if(parametros[2].size() == 2 && parametros[2][0] == '+')
                                {
                                    if (PlusConfig(clients, i, parametros, clients[i].getChannel(j), buff))
                                        ListenEverybody(clients, clients[i].getChannel(j), std::string(buff) + "\n", 0);
                                }
                                else 
                                    SendMsg(clients, i, "Unknown MODE flag\n");
                                return ;
                            }
                        }
                        SendMsg(clients, i, parametros[1] + ": You're not channel operator\n");
                        return ;
                    }
                }
                SendMsg(clients, i, parametros[1] + ": You're not on that channel\n");
            }
            else
                SendMsg(clients, i, parametros[1] + ": No such channel\n");
        }
        else
            SendMsg(clients, i, parametros[1] + ": Bad Channel Mask\n");
    }
    else
        SendMsg(clients, i, std::string(buff) + ": Erroneous parameters\n");
}

void ConfigTopic(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, Channel *channel, char buff[1024])
{
    if (parametros.size() == 2)
    {
        if (!channel->getTopic()->getTopic().empty())
        {
            SendMsg(clients, i, parametros[1] + " :" + channel->getTopic()->getTopic() + "\n");
            SendMsg(clients, i, parametros[1] + " Topic Athor:" +channel->getTopic()->getAuthor() + "\n");
        }
        else
            SendMsg(clients, i, parametros[1] + ": No topic is set\n");
    }
    else
    {
        std::string topic = FtColons(buff);
        if (topic.length() > 390)
        {
            SendMsg(clients, i, topic + ": Topic too long\n");
            return ;
        }
        Topic tempTopic(topic, clients[i].getNick());
        channel->setTopic(tempTopic);
        ListenEverybody(clients, channel, std::string(buff) + "\n", 0);
    }
}

void CmdTOPIC(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024])
{
    if (!clients[i].getPasswd() || clients[i].getNick().empty() || clients[i].getUser()->getUserName().empty())
    {
        SendMsg(clients, i, "You have not registered\n");
        return;
    }
    if (parametros.size() >= 2)
    {
        if (parametros.size() > 2)
        {
            if (parametros[2][0] != ':')
            {
                SendMsg(clients, i, std::string(buff) + ": Erroneous parameters\n");
                return;
            }
        }
        if (parametros[1][0] == '&' || parametros[1][0] == '#')
        {
            if(DoesExist(clients, parametros[1]))
            {
                for (size_t j = 0; j < clients[i].getChannels().size(); ++j)
                {
                    if (clients[i].getChannel(j)->getName() == parametros[1])
                    {
                        if(clients[i].getChannel(j)->getTopicProtected())
                        {
                            for (size_t k = 0; k < clients[i].getChannel(j)->getOperators().size(); ++k)
                            {
                                if (clients[i].getChannel(j)->getOperator(k) == clients[i].getNick())
                                {
                                    ConfigTopic(clients, i, parametros, clients[i].getChannel(j), buff);
                                    return ;
                                }
                            }
                            SendMsg(clients, i, parametros[1] + ": You're not channel operator\n");
                        }
                        else
                            ConfigTopic(clients, i, parametros, clients[i].getChannel(j), buff);
                        return ;
                    }
                }
                SendMsg(clients, i, parametros[1] + ": You're not on that channel\n");
                return ;
            }
            else
                SendMsg(clients, i, parametros[1] + ": No such channel\n");
        }
        else
            SendMsg(clients, i, parametros[1] + ": Bad Channel Mask\n");
    }
    else
        SendMsg(clients, i, std::string(buff) + ": Not enough parameters\n");
}

void ConfigInvite(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024])
{
    for (size_t j = 0; j < clients.size(); ++j)
    {
        if(parametros[1] == clients[j].getNick())
        {
            for (size_t k = 0; k < clients[j].getChannels().size(); ++k)
            {
                if (clients[j].getChannel(k)->getName() == parametros[2])
                {
                    SendMsg(clients, i, parametros [1] + " " + parametros[2] + ": is already on channel\n");
                    return ;
                }
            }
            if(clients[j].setInvitation(parametros[2]))
            {
                SendMsg(clients, i, parametros[1] + " " + parametros[2] + "\n");
                SearchTarget(clients, parametros[1], std::string(buff) + "\n", i);
            }
        }
    }
}

void CmdINVITE(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024])
{
    if (clients[i].getPasswd() == false || clients[i].getNick().empty() || clients[i].getUser()->getUserName().empty())
        SendMsg(clients, i, "You have not registered\n");
    else if (parametros.size() < 3)
        SendMsg(clients, i, std::string(buff) + ": Not enough parameters\n");
    else if (parametros.size() == 3)
    {
        if(DoesExist(clients, parametros[1]))
        {
            if(parametros[1] != clients[i].getNick())
            {
                if (parametros[2][0] == '&' || parametros[2][0] == '#')
                {
                    if(DoesExist(clients, parametros[2]))
                    {
                        for (size_t j = 0; j < clients[i].getChannels().size(); ++j)
                        {
                            if (clients[i].getChannel(j)->getName() == parametros[2])
                            {
                                if(clients[i].getChannel(j)->getInviteOnly())
                                {
                                    for (size_t k = 0; k < clients[i].getChannel(j)->getOperators().size(); ++k)
                                    {
                                        if (clients[i].getChannel(j)->getOperator(k) == clients[i].getNick())
                                        {
                                            ConfigInvite(clients, i, parametros, buff);
                                            return ;
                                        }
                                    }
                                    SendMsg(clients, i, parametros[2] + ": You're not channel operator\n");
                                }
                                else
                                    ConfigInvite(clients, i, parametros, buff);
                                return ;
                            }
                        }
                        SendMsg(clients, i, parametros[2] + ": You're not on that channel\n");
                        return ;
                    }
                    else
                        SendMsg(clients, i, parametros[1] + ": No such channel\n");
                }
                else
                    SendMsg(clients, i, parametros[2] + ": Bad Channel Mask\n");
            }
            else
                SendMsg(clients, i, parametros[1] + ": You can't invite yourself\n");
        }
        else
            SendMsg(clients, i, parametros[1] + ": No such nick\n");
    }
    else
        SendMsg(clients, i, std::string(buff) + ": Erroneous parameters\n");
}


int ConfigKick(std::vector<Client>& clients, Channel *channel, std::string nick, std::vector<std::string> parametros, char buff[1024])
{
    for (size_t k = 0; k < clients.size(); ++k)
    {
        if (nick == clients[k].getNick())
        {
            std::string msg = FtColons(buff);
            clients[k].removeChannel(channel);
            if(!msg.empty())
                msg = ": " + msg;
            SendMsg(clients, k, parametros[0] + " " + parametros[1] + " " + nick + msg + "\n");
            ListenEverybody(clients, channel, parametros[0] + " " + parametros[1] + " " + nick + msg + "\n", 0);
            return 1;
        }
    }
    return 0;
}

void CmdKICK(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024])
{   
    if (clients[i].getPasswd() == false || clients[i].getNick().empty() || clients[i].getUser()->getUserName().empty())
        SendMsg(clients, i, "You have not registered\n");
    if (parametros.size() >= 3) 
    {
        if (parametros.size() > 3)
        {
            if (parametros[3][0] != ':')
            {
                SendMsg(clients, i, std::string(buff) + ": Erroneous parameters\n");
                return;
            }
        }
        if (parametros[1][0] == '&' || parametros[1][0] == '#')
        {
            if(DoesExist(clients, parametros[1]))
            {
                for (size_t j = 0; j < clients[i].getChannels().size(); ++j)
                {
                    if (clients[i].getChannel(j)->getName() == parametros[1])
                    {
                        for (size_t k = 0; k < clients[i].getChannel(j)->getOperators().size(); ++k)
                        {
                            if (clients[i].getChannel(j)->getOperator(k) == clients[i].getNick())
                            {
                                std::vector<std::string> temp_users = ft_split(parametros[2].c_str(), ',');
                                for(size_t l = 0; l < temp_users.size(); ++l)
                                {
                                    if(temp_users[l] == clients[i].getChannel(j)->getFounder())
                                        SendMsg(clients, i, temp_users[l] + ": Cannot kick channel founder\n");
                                    else if(!ConfigKick(clients, clients[i].getChannel(j), temp_users[l], parametros, buff))
                                        SendMsg(clients, i, temp_users[l] + ": No such nick\n");
                                }
                                return ;
                            }
                        }
                        SendMsg(clients, i, parametros[1] + ": You're not channel operator\n");
                        return ;
                    }
                }
                SendMsg(clients, i, parametros[1] + ": You're not on that channel\n");
                return ;
            }
            else
                SendMsg(clients, i, parametros[1] + ": No such channel\n");
        }
        else
            SendMsg(clients, i, parametros[1] + ": Bad Channel Mask\n");
    }
    else
        SendMsg(clients, i, std::string(buff) + ": Not enough parameters\n");
}
