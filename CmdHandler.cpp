#include "CmdHandler.hpp"

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

void SendWelcome(std::vector<Client>& clients, size_t i)
{
    if (!clients[i].getNick().empty() && !clients[i].getUser()->getUserName().empty())
    {
        SendMsg(clients, i, ":ircserver 001 " + clients[i].getNick() + " :Welcome to the IRC server " + clients[i].getNick() + "\r\n");
		SendMsg(clients, i, ":ircserver 002 " + clients[i].getNick() + " :Your host is ircserver, running version 1.0\r\n");
		SendMsg(clients, i, ":ircserver 003 " + clients[i].getNick() + " :This server was created today\r\n");
		SendMsg(clients, i, ":ircserver 004 " + clients[i].getNick() + " ircserver 1.0 o o\r\n");
    }
}

void CmdPASS(std::vector<Client>& clients, std::string Passwd, size_t i, std::vector<std::string> parametros, char buff[1024])
{
	if (parametros.size() < 2)
        SendMsg(clients, i, ":ircserver 461 " +  (clients[i].getNick().empty() ? "*" : clients[i].getNick()) + " " + std::string(buff) + " :Not enough parameters\r\n");
	else if (parametros.size() != 2)
        SendMsg(clients, i, ":ircserver 461 " +  (clients[i].getNick().empty() ? "*" : clients[i].getNick()) + " " + std::string(buff) + " :Erroneous parameters\r\n");
	else
	{
		if((parametros[1] == Passwd))
		{
			if (clients[i].getPasswd() == true)
			{
				SendMsg(clients, i, ":ircserver 462 " +  (clients[i].getNick().empty() ? "*" : clients[i].getNick()) + " :You may not reregister\r\n");
				return ;
			}
			clients[i].setPasswd(true);
		}
		else
            SendMsg(clients, i, ":ircserver 464 " +  (clients[i].getNick().empty() ? "*" : clients[i].getNick()) + " :Password incorrect\r\n");
	}
}

void CmdNICK(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024])
{
	if(clients[i].getPasswd() == false)
        SendMsg(clients, i, ":ircserver 451 " + (clients[i].getNick().empty() ? "*" : clients[i].getNick()) + " :You have not registered\r\n");
	else if (parametros.size() == 1)
        SendMsg(clients, i, ":ircserver 431 " + (clients[i].getNick().empty() ? "*" : clients[i].getNick()) + " :No nickname given\r\n");
	else if (parametros.size() != 2)
        SendMsg(clients, i, ":ircserver 461 " + (clients[i].getNick().empty() ? "*" : clients[i].getNick()) + " " + std::string(buff) + " :Erroneous parameters\r\n");
	else if((parametros[1].size() <= 9) && !ForbiddenChars(parametros[1]) && !std::isdigit(parametros[1][0]))
	{
		for (size_t j = 0; j < clients.size(); ++j)
		{
            if (j != i && parametros[1] == clients[j].getNick())
			{
                SendMsg(clients, i, ":ircserver 433 " + (clients[i].getNick().empty() ? "*" : clients[i].getNick()) + " " + parametros[1] + " :Nickname is already in use\r\n");
                return ;
            }
        }
		if(!clients[i].getNick().empty())
		{
			SendMsg(clients, i, ":" + clients[i].getNick() + " NICK " + parametros[1] + "\r\n");
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
                ListenEverybody(clients, clients[i].getChannel(j), ":" + clients[i].getNick() + "!" + (clients[i].getUser()->getUserName().empty() ? "*" : clients[i].getUser()->getUserName()) + "@" + clients[i].getIp() + " NICK " + parametros[1] + "\r\n", 0);
			}
		}
        clients[i].setNick(parametros[1]);
	}
	else
        SendMsg(clients, i, ":ircserver 432 " + (clients[i].getNick().empty() ? "*" : clients[i].getNick()) + " " + parametros[1] + " :Erroneous nickname\r\n");
}

void CmdUSER(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024])
{
	if ((clients[i].getPasswd() == false ) || (clients[i].getNick().empty()))
        SendMsg(clients, i, ":ircserver 451 " + (clients[i].getNick().empty() ? "*" : clients[i].getNick()) + " :You have not registered\r\n");
	else if ((parametros.size() < 5) || (parametros.size() == 5 && parametros[4].size() == 1 && parametros[4][0] == ':'))
        SendMsg(clients, i, ":ircserver 461 " + (clients[i].getNick().empty() ? "*" : clients[i].getNick()) + " " + std::string(buff) + " :Not enough parameters\r\n");
	else if ((!ForbiddenChars(parametros[1])) && \
	(parametros[2] == "0") && \
	(parametros[3] == "*") && \
	(parametros[4][0] == ':'))
	{
        std::string realname = FtColons(buff);
		if(!clients[i].getUser()->getUserName().empty())
		{	
			SendMsg(clients, i, ":ircserver 462 " + clients[i].getNick() + " :You may not reregister\r\n");
			return ;
		}
		User user(parametros[1], realname, atoi(parametros[2].c_str()));
		clients[i].setUser(user);
		SendWelcome(clients, i);
	}
	else
    SendMsg(clients, i, ":ircserver 461 " + (clients[i].getNick().empty() ? "*" : clients[i].getNick()) + " " + std::string(buff) + " :Erroneous parameters\r\n");
}

void ConfigJoin(std::vector<Client>& clients, size_t i, std::vector<std::string> temp_channels, size_t j, size_t k, size_t l)
{
    if(!clients[k].getChannel(l)->getTopic()->getTopic().empty())
        SendMsg(clients, i, ":ircserver 332 " + clients[i].getNick() + " " + temp_channels[j] + " :" + clients[k].getChannel(l)->getTopic()->getTopic() + "\r\n");
    SendMsg(clients, i, ":ircserver 353 " + clients[i].getNick() + " " + temp_channels[j] + " :");
    std::string user_list;
    for (size_t m = 0; m < clients[k].getChannel(l)->getUsers().size(); ++m)
        user_list += clients[k].getChannel(l)->getUser(m) + " ";
    SendMsg(clients, i, user_list + "\r\n");
    SendMsg(clients, i, ":ircserver 366 " + clients[i].getNick() + " " + temp_channels[j] + " :End of /NAMES list\r\n");
    clients[k].getChannel(l)->setUser(clients[i].getNick());
    clients[i].setChannel(clients[k].getChannel(l));
    ListenEverybody(clients, clients[k].getChannel(l), ":" + clients[i].getNick() + "!" + clients[i].getUser()->getUserName() + "@" + clients[i].getIp() + " JOIN " + temp_channels[j] + "\r\n", 0);
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
                    SendMsg(clients, i, ":ircserver 471 " + clients[i].getNick() + " " + temp_channels[j] + " :Cannot join channel (+l) - Channel is full\r\n");
            }
            else
                ConfigJoin(clients, i, temp_channels, j, k, l);
        }
        else
            SendMsg(clients, i, ":ircserver 473 " + clients[i].getNick() + " " + temp_channels[j] + " :Cannot join channel (+i) - You must be invited\r\n");
    }
    else
    {
        if(clients[k].getChannel(l)->getLimitChannel())
        {
            if(clients[k].getChannel(l)->getUsers().size() < clients[k].getChannel(l)->getLimit())   
                ConfigJoin(clients, i, temp_channels, j, k, l);
            else
                SendMsg(clients, i, ":ircserver 471 " + clients[i].getNick() + " " + temp_channels[j] + " :Cannot join channel (+l) - Channel is full\r\n");
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
                        SendMsg(clients, i, ":ircserver 475 " + clients[i].getNick() + " " + temp_channels[j] + " :Cannot join channel (+k) - Bad channel key\r\n");
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
        SendMsg(clients, i, ":ircserver 451 " + (clients[i].getNick().empty() ? "*" : clients[i].getNick()) + " :You have not registered\r\n");
    else if (parametros.size() < 2)
        SendMsg(clients, i, ":ircserver 461 " + clients[i].getNick() + " " + std::string(buff) + " :Not enough parameters\r\n");
	else if (parametros.size() >= 2 && parametros.size() <= 3)
	{
		if(parametros[1] == "0")
		{
			for(size_t j = 0; j < clients[i].getChannels().size(); ++j)
				ListenEverybody(clients, clients[i].getChannel(j), ":" + clients[i].getNick() + "!" + clients[i].getUser()->getUserName() + "@" + clients[i].getIp() + " PART " + clients[i].getChannel(j)->getName() + "\r\n", i);
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
                        SendMsg(clients, i, ":ircserver 324 " + clients[i].getNick() + " " + temp_channels[j] + "\r\n");
                    }
                    else
                        SendMsg(clients, i, ":ircserver 479 " + clients[i].getNick() + " " + temp_channels[j] + " :Invalid channel name (too long)\r\n");
                }
			}
			else
                SendMsg(clients, i, ":ircserver 476 " + clients[i].getNick() + " " + temp_channels[j] + " :Bad Channel Mask\r\n");
		}
	}
	else
        SendMsg(clients, i, ":ircserver 461 " + clients[i].getNick() + " " + std::string(buff) + " :Erroneous parameters\r\n");
}

void CmdPRIVMSG(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024])
{
	if((clients[i].getPasswd() == false ) ||(clients[i].getNick().empty()) || (clients[i].getUser()->getUserName().empty()))
        SendMsg(clients, i, ":ircserver 451 " + (clients[i].getNick().empty() ? "*" : clients[i].getNick()) + " :You have not registered\r\n");
	else if (parametros.size() < 3)
        SendMsg(clients, i, ":ircserver 461 " + clients[i].getNick() + " " + std::string(buff) + " :Not enough parameters\r\n");
	else if ((!ForbiddenJoin(parametros[1])) && (parametros[2][0] == ':'))
	{
		std::vector<std::string> temp_target = ft_split(parametros[1].c_str(), ',');
        std::string temp_msg = FtColons(buff);
		if(temp_msg.empty())
            SendMsg(clients, i, ":ircserver 412 " + clients[i].getNick() + " :No text to send\r\n");
        else if (temp_msg.length() > 512)
            SendMsg(clients, i, ":ircserver 416 " + clients[i].getNick() + " " + temp_msg + " :Topic too long\r\n");
        else
        {
            for(size_t j = 0; j < temp_target.size(); ++j)
                if(!SearchTarget(clients, temp_target[j], ":" + clients[i].getNick() + "!" + clients[i].getUser()->getUserName() + "@" + clients[i].getIp() + " PRIVMSG " + temp_target[j] + " :" + temp_msg + "\r\n", i))
                    SendMsg(clients, i, ":ircserver 401 " + clients[i].getNick() + " " + temp_target[j] + " :No such nick/channel\r\n");
        }
	}
	else
        SendMsg(clients, i, ":ircserver 461 " + clients[i].getNick() + " " + std::string(buff) + " :Erroneous parameters\r\n");
}

void ListModes(std::vector<Client>& clients, size_t i, Channel *channel)
{
    std::string msg;
    if(channel->getTopicProtected())
        msg = msg + "t";
    if(channel->getInviteOnly())
        msg = msg + "i";
    if(channel->getLimitChannel())
    {
        std::stringstream lim;
        lim << channel->getLimit();
        msg = "l" + msg + " " + lim.str();
    }
    if(channel->getPasswd().size() != 0)
        msg = "k" + msg + " " + channel->getPasswd();
    if(msg.size() != 0)
        msg = " +" + msg;
    SendMsg(clients, i, ":ircserver 324 " + clients[i].getNick() + " " + channel->getName() + msg + "\r\n");
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
            SendMsg(clients, i, ":ircserver 461 " + clients[i].getNick() + " " + std::string(buff) + " :Not enough parameters\r\n");
            return 0;
        }
        else if (!DoesExist(clients, parametros[3]))
        {
            SendMsg(clients, i, ":ircserver 401 " + clients[i].getNick() + " " + parametros[3] + " :No such nick\r\n");
            return 0;
        }
        else if (parametros[3] == channel->getFounder())
        {
            SendMsg(clients, i, ":ircserver 484 " + clients[i].getNick() + " " + parametros[3] + " :Cannot remove privileges from channel founder\r\n");
            return 0;
        }
        else
            channel->removeOperator(parametros[3]);
    }
    else
    {
        SendMsg(clients, i, ":ircserver 472 " + clients[i].getNick() + " " + std::string(buff) + " :Unknown MODE flag\r\n");
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
            SendMsg(clients, i, ":ircserver 461 " + clients[i].getNick() + " " + std::string(buff) + " :Not enough parameters\r\n");
        else if (parametros[3].find_first_not_of("0123456789") != std::string::npos || std::atoi(parametros[3].c_str()) < 1 || 2147483647 < std::atoi(parametros[3].c_str()))
            SendMsg(clients, i, ":ircserver 999 " + clients[i].getNick() + " " + parametros[3] + " :Invalid limit\r\n");
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
            SendMsg(clients, i, ":ircserver 461 " + clients[i].getNick() + " " + std::string(buff) + " :Not enough parameters\r\n");
        else if (parametros[3].length() > 23)
            SendMsg(clients, i, ":ircserver 999 " + clients[i].getNick() + " " + parametros[3] + " :Password too long\r\n");
        else
        {
            channel->setPasswd(parametros[3]);
            return 1;
        }
    }
    else if(parametros[2][1] == 'o')
    {
        if (parametros.size() != 4)
            SendMsg(clients, i, ":ircserver 461 " + clients[i].getNick() + " " + std::string(buff) + " :Not enough parameters\r\n");
        else if(!DoesExist(clients, parametros[3]))
            SendMsg(clients, i, ":ircserver 401 " + clients[i].getNick() + " " + parametros[3] + " :No such nick\r\n");
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
            SendMsg(clients, i, ":ircserver 442 " + clients[i].getNick() + " " + parametros[3] + " :You're not on that channel\r\n");
        }
    }
    else
        SendMsg(clients, i, ":ircserver 472 " + clients[i].getNick() + " " + std::string(buff) + " :Unknown MODE flag\r\n");
    return 0;
}

void CmdMODE(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024])
{
    if (clients[i].getPasswd() == false || clients[i].getNick().empty() || clients[i].getUser()->getUserName().empty())
        SendMsg(clients, i, ":ircserver 451 " + (clients[i].getNick().empty() ? "*" : clients[i].getNick()) + " :You have not registered\r\n");
    else if (parametros.size() < 2)
        SendMsg(clients, i, ":ircserver 461 " + clients[i].getNick() + " " + std::string(buff) + " :Not enough parameters\r\n");
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
                                        ListenEverybody(clients, clients[i].getChannel(j), ":" + clients[i].getNick() + "!" + clients[i].getUser()->getUserName() + "@" + clients[i].getIp() + " " + buff + "\r\n", 0);
                                }
                                else if(parametros[2].size() == 2 && parametros[2][0] == '+')
                                {
                                    if (PlusConfig(clients, i, parametros, clients[i].getChannel(j), buff))
                                        ListenEverybody(clients, clients[i].getChannel(j), ":" + clients[i].getNick() + "!" + clients[i].getUser()->getUserName() + "@" + clients[i].getIp() + " " + buff + "\r\n", 0);
                                }
                                else 
                                    SendMsg(clients, i, ":ircserver 472 " + clients[i].getNick() + " " + std::string(buff) + " :Unknown MODE flag\r\n");
                                return ;
                            }
                        }
                        SendMsg(clients, i, ":ircserver 482 " + clients[i].getNick() + " " + parametros[1] + " :You're not channel operator\r\n");
                        return ;
                    }
                }
                SendMsg(clients, i, ":ircserver 442 " + clients[i].getNick() + " " + parametros[1] + " :You're not on that channel\r\n");
            }
            else
                SendMsg(clients, i, ":ircserver 403 " + clients[i].getNick() + " " + parametros[1] + " :No such channel\r\n");
        }
        else
            SendMsg(clients, i, ":ircserver 476 " + clients[i].getNick() + " " + parametros[1] + " :Bad Channel Mask\r\n");
    }
    else
        SendMsg(clients, i, ":ircserver 461 " + clients[i].getNick() + " " + std::string(buff) + " :Erroneous parameters\r\n");
}

void ConfigTopic(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, Channel *channel, char buff[1024])
{
    if (parametros.size() == 2)
    {
        if (!channel->getTopic()->getTopic().empty())
        {
            SendMsg(clients, i, ":ircserver 332 " + clients[i].getNick() + " " + parametros[1] + " :" + channel->getTopic()->getTopic() + "\r\n");
            SendMsg(clients, i, ":ircserver 333 " + clients[i].getNick() + " " + parametros[1] + " " + channel->getTopic()->getAuthor() + "\r\n");
        }
        else
            SendMsg(clients, i, ":ircserver 331 " + clients[i].getNick() + " " + parametros[1] + " :No topic is set\r\n");
    }
    else
    {
        std::string topic = FtColons(buff);
        if (topic.length() > 390)
        {
            SendMsg(clients, i, ":ircserver 416 " + clients[i].getNick() + " " + topic + " :Topic too long\r\n");
            return ;
        }
        Topic tempTopic(topic, clients[i].getNick());
        channel->setTopic(tempTopic);
        ListenEverybody(clients, channel, ":" + clients[i].getNick() + "!" + clients[i].getUser()->getUserName() + "@" + clients[i].getIp() + " TOPIC " + channel->getName() + " :" + topic + "\r\n", 0);
    }
}

void CmdTOPIC(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024])
{
    if (!clients[i].getPasswd() || clients[i].getNick().empty() || clients[i].getUser()->getUserName().empty())
    {
        SendMsg(clients, i, ":ircserver 451 " + (clients[i].getNick().empty() ? "*" : clients[i].getNick()) + " :You have not registered\r\n");
        return;
    }
    if (parametros.size() >= 2)
    {
        if (parametros.size() > 2)
        {
            if (parametros[2][0] != ':')
            {
                SendMsg(clients, i, ":ircserver 461 " + clients[i].getNick() + " " + std::string(buff) + " :Erroneous parameters\r\n");
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
                            SendMsg(clients, i, ":ircserver 482 " + clients[i].getNick() + " " + parametros[1] + " :You're not channel operator\r\n");
                        }
                        else
                            ConfigTopic(clients, i, parametros, clients[i].getChannel(j), buff);
                        return ;
                    }
                }
                SendMsg(clients, i, ":ircserver 442 " + clients[i].getNick() + " " + parametros[1] + " :You're not on that channel\r\n");
                return ;
            }
            else
                SendMsg(clients, i, ":ircserver 403 " + clients[i].getNick() + " " + parametros[1] + " :No such channel\r\n");
        }
        else
            SendMsg(clients, i, ":ircserver 476 " + clients[i].getNick() + " " + parametros[1] + " :Bad Channel Mask\r\n");
    }
    else
        SendMsg(clients, i, ":ircserver 461 " + clients[i].getNick() + " " + std::string(buff) + " :Not enough parameters\r\n");
}

void ConfigInvite(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros)
{
    for (size_t j = 0; j < clients.size(); ++j)
    {
        if(parametros[1] == clients[j].getNick())
        {
            for (size_t k = 0; k < clients[j].getChannels().size(); ++k)
            {
                if (clients[j].getChannel(k)->getName() == parametros[2])
                {
                    SendMsg(clients, i, ":ircserver 443 " + clients[i].getNick() + " " + parametros[1] + " " + parametros[2] + " :is already on channel\r\n");
                    return ;
                }
            }
            if(clients[j].setInvitation(parametros[2]))
            {
                SendMsg(clients, i, ":ircserver 341 " + clients[i].getNick() + " " + parametros[1] + " " + parametros[2] + "\r\n");
                SearchTarget(clients, parametros[1], ":" + clients[i].getNick() + "!" + clients[i].getUser()->getUserName() + "@" + clients[i].getIp() + " INVITE " + parametros[1] + " :" + parametros[2] + "\r\n", i);
            }
        }
    }
}

void CmdINVITE(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024])
{
    if (clients[i].getPasswd() == false || clients[i].getNick().empty() || clients[i].getUser()->getUserName().empty())
        SendMsg(clients, i, ":ircserver 451 " + (clients[i].getNick().empty() ? "*" : clients[i].getNick()) + " :You have not registered\r\n");
    else if (parametros.size() < 3)
        SendMsg(clients, i, ":ircserver 461 " + clients[i].getNick() + " " + std::string(buff) + " :Not enough parameters\r\n");
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
                                            ConfigInvite(clients, i, parametros);
                                            return ;
                                        }
                                    }
                                    SendMsg(clients, i, ":ircserver 482 " + clients[i].getNick() + " " + parametros[2] + " :You're not channel operator\r\n");
                                }
                                else
                                    ConfigInvite(clients, i, parametros);
                                return ;
                            }
                        }
                        SendMsg(clients, i, ":ircserver 442 " + clients[i].getNick() + " " + parametros[2] + " :You're not on that channel\r\n");
                        return ;
                    }
                    else
                        SendMsg(clients, i, ":ircserver 403 " + clients[i].getNick() + " " + parametros[2] + " :No such channel\r\n");
                }
                else
                    SendMsg(clients, i, ":ircserver 476 " + clients[i].getNick() + " " + parametros[2] + " :Bad Channel Mask\r\n");
            }
            else
                SendMsg(clients, i, ":ircserver 999 " + clients[i].getNick() + " " + parametros[1] + " :You can't invite yourself\r\n");
        }
        else
            SendMsg(clients, i, ":ircserver 401 " + clients[i].getNick() + " " + parametros[1] + " :No such nick\r\n");
    }
    else
        SendMsg(clients, i, ":ircserver 461 " + clients[i].getNick() + " " + std::string(buff) + " :Erroneous parameters\r\n");
}


int ConfigKick(std::vector<Client>& clients, size_t i ,Channel *channel, std::string nick, std::vector<std::string> parametros, char buff[1024])
{
    for (size_t k = 0; k < clients.size(); ++k)
    {
        if (nick == clients[k].getNick())
        {
            std::string msg = FtColons(buff);
            if(!msg.empty())
                msg = ": " + msg;
            ListenEverybody(clients, channel, ":" + clients[i].getNick() + "!" + clients[i].getUser()->getUserName() + "@" + clients[i].getIp() + " " + parametros[0] + " " + parametros[1] + " " + nick + msg + "\r\n", 0);
            clients[k].removeChannel(channel);
            return 1;
        }
    }
    return 0;
}

void CmdKICK(std::vector<Client>& clients, size_t i, std::vector<std::string> parametros, char buff[1024])
{   
    if (clients[i].getPasswd() == false || clients[i].getNick().empty() || clients[i].getUser()->getUserName().empty())
        SendMsg(clients, i, ":ircserver 451 " + (clients[i].getNick().empty() ? "*" : clients[i].getNick()) + " :You have not registered\r\n");
    if (parametros.size() >= 3) 
    {
        if (parametros.size() > 3)
        {
            if (parametros[3][0] != ':')
            {
                SendMsg(clients, i, ":ircserver 461 " + clients[i].getNick() + " " + std::string(buff) + " :Erroneous parameters\r\n");
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
                                        SendMsg(clients, i, ":ircserver 484 " + clients[i].getNick() + " " + temp_users[l] + " :Cannot kick channel founder\r\n");
                                    else if(!ConfigKick(clients, i, clients[i].getChannel(j), temp_users[l], parametros, buff))
                                        SendMsg(clients, i, ":ircserver 401 " + clients[i].getNick() + " " + temp_users[l] + " :No such nick\r\n");
                                }
                                return ;
                            }
                        }
                        SendMsg(clients, i, ":ircserver 482 " + clients[i].getNick() + " " + parametros[1] + " :You're not channel operator\r\n");
                        return ;
                    }
                }
                SendMsg(clients, i, ":ircserver 442 " + clients[i].getNick() + " " + parametros[1] + " :You're not on that channel\r\n");
                return ;
            }
            else
                SendMsg(clients, i, ":ircserver 403 " + clients[i].getNick() + " " + parametros[1] + " :No such channel\r\n");
        }
        else
            SendMsg(clients, i, ":ircserver 476 " + clients[i].getNick() + " " + parametros[1] + " :Bad Channel Mask\r\n");
    }
    else
        SendMsg(clients, i, ":ircserver 461 " + clients[i].getNick() + " " + std::string(buff) + " :Not enough parameters\r\n");
}
