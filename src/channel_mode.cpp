#include "Channel.hpp"
#include "Server.hpp"
#include "Commands.hpp"


static int	mode_get(Client &client)
{
	std::string	modeString = "";
	std::string	modeArgs = "";
	if (_isInviteOnly)
		modeString += "i";
	if (_restrictTopic)
		modeString += "t";
	if (_clientLimit > 0) {
		modeString += "l";
		std::stringstream ss;
		ss << _clientLimit;
		modeArgs += " " + ss.str();
	}
	if (!_key.empty()) {
		modeString += "k";
		modeArgs += " " + _key;
	}
	if (!modeString.empty())
		modeString = "+" + modeString;

	std::string	message = modeString + modeArgs;
	_server->sendToClient(buildReply(_name, client.getNickname(), 324, message, 1, _name.c_str()), client);
	return (0);
}

static int mode_invite(const std::string& channelName, const std::string& modeString, const std::string &arg,  Client &client)
{
    if (modeString == "-i")
	{
		channel->setInviteOnly(false);
		sendToClient(buildReply(name, channelName.c_str(), MODE, "", 1, modeString.c_str()), client);
		sendToChannel(buildReply(name, channelName.c_str(), MODE, "", 1, modeString.c_str()),*channel, client);
        return (1);
	}
	if (modeString == "+i")
	{
		channel->setInviteOnly(true);
		sendToClient(buildReply(name, channelName.c_str(), MODE, "", 1, modeString.c_str()), client);
		sendToChannel(buildReply(name, channelName.c_str(), MODE, "", 1, modeString.c_str()),*channel, client);
        return (1);
	}
    return (0);
}

static int mode_topic(const std::string& channelName, const std::string& modeString, const std::string &arg,  Client &client)
{
    if (modeString == "-t")
	{
		channel->setrestrictTopic(false);
		sendToClient(buildReply(name, channelName.c_str(), MODE, "", 1, modeString.c_str()), client);
		sendToChannel(buildReply(name, channelName.c_str(), MODE, "", 1, modeString.c_str()),*channel, client);
        return (1);
	}
	if (modeString == "+t")
	{
		channel->setrestrictTopic(true);
		sendToClient(buildReply(name, channelName.c_str(), MODE, "", 1, modeString.c_str()), client);
		sendToChannel(buildReply(name, channelName.c_str(), MODE, "", 1, modeString.c_str()),*channel, client);
        return (1);
	}
    return (0);
}

static int mode_key(const std::string& channelName, const std::string& modeString, const std::string &arg,  Client &client)
{
    if (modeString == "-k")
    {
        channel->setKey("");
		sendToClient(buildReply(name, channelName.c_str(), MODE, "", 1, modeString.c_str()), client);
        return (1);
    }
    if (modeString == +"k")
    {
        if (arg.empty())
            return (0); //send not enought arguments 
		channel->setKey(arg);
		sendToClient(buildReply(name, channelName.c_str(), MODE, "", 2, modeString.c_str(), arg.c_str()), client);
        return (1);
    }
    return (0);
}

static int mode_op(const std::string& channelName, const std::string& modeString, const std::string& arg, Client &client)
{
    if (modeString == "+o")
    {
        if (!channel->clientIsOp(arg))
        {
            if (!findClient(arg))
                return (sendToClient(buildReply(SERVER, client.getNickname(), 401, "", 1, arg.c_str()), client)); //wrong error 
            channel->addOperator(*findClient(arg));
            channel->removeClient(arg);
            sendToClient(buildReply(name, channelName.c_str(), MODE, "", 2, modeString.c_str(), arg.c_str()), client);
            sendToChannel(buildReply(name, channelName.c_str(), MODE, "", 2, modeString.c_str(), arg.c_str()),*channel, client);
            return(1);
        }
        return (0);
    }
    if (modeString == "-o")
    {
        if (channel->clientIsOp(arg))
        {
            if (!channel->findOps(arg))
                return (sendToClient(buildReply(SERVER, client.getNickname(), 401, "", 1, arg.c_str()), client)); //wrong error
            channel->removeOperator(arg);
            channel->addClient(*findClient(arg));
            sendToClient(buildReply(name, channelName.c_str(), MODE, "", 2, modeString.c_str(), arg.c_str()), client);
            sendToChannel(buildReply(name, channelName.c_str(), MODE, "", 2, modeString.c_str(), arg.c_str()),*channel, client);
            return (1);
        }
        return (0);
    }
    return (0);
}

static int mode_limit(const std::string& channelName, const std::string& modeString, const std::string &arg,  Client &client)
{
    if (arg.empty())
        return (0);
    if(modeString == "+l")
    {
		channel->setClientLimit(atoi(arg.c_str()));
		sendToClient(buildReply(name, channelName.c_str(), MODE, "", 2, modeString.c_str(), arg.c_str()), client);
        return (1);
	}
	if (modeString == "-l")
	{
		channel->setClientLimit(0);
		sendToClient(buildReply(name, channelName.c_str(), MODE, "", 2, modeString.c_str(), arg.c_str()), client);
        return (1);
	}
    return (0);
}

int	Channel::mode(const std::string& channelName, const std::string& modeString, const std::string &arg,  Client &client)
{
	std::string name = client.getNickname();

	if(channelName.empty())
		return (_server->sendToClient(buildReply(SERVER, name, 461, "", 1, "PRIVMSG"), client));
	Channel *channel = findChannel(channelName);

    if (modeString.empty())
        return (get_Mode(client));
	if (channel == NULL)
		return (sendToClient(buildReply(SERVER, name, 403, "", 1, channelName.c_str()), client));
	if (!channel->clientIsOp(name))
		return(sendToClient(buildReply(SERVER, client.getNickname(), 482, "", 1, channelName.c_str()), client));

	if (modeString.empty())
		return (channel->checkMode(client));
	if ((modeString == "-i" || modeString == "+i") && mode_invite(channelName, modeString, arg, client))
        return (0);
    else
        return (1); //failure
	if ((modeString == "-t" || modeString == "+t") && mode_topic(channelName, modeString, arg, client))
        return (0);
    else
        return (1); //failure
	if ((modeString == "-k" || modeString == "+k") && mode_key(channelName, modeString, arg, client))
        return (0);
    else
        return (1); //failure
	if (modeString == "+o" || modeString == "-o" && mode_op(channelName, modeString, arg, client))
		return (0);
    else
        return (1); //failure
	if ((modeString == "+l" || modeString == "-l") && mode_limit(channelName, modeString, arg, client))
        return (0);
    else
        return (1); //failure
	return (0);
}