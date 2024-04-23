#include "IRC.hpp"


Server::Server(const std::string& port, const std::string& password, const std::string& apikey) : _port(port), _password(password), _apikey(apikey)
{
	_running = false;
	guestCount = 0;
	_bot = new ChatGPT(apikey);
	memset(&_hints, 0, sizeof(_hints));
}

Server::~Server()
{
	closeSockets();
	free(_serverInfo);
	std::cout << "Shutting down" << std::endl;
}

void	Server::start()
{
	std::cout << GREEN << "Server starting" << RESET << std::endl;
	createSocket();
	_running = true;
	signal(SIGINT, signalHandler);
	signal(SIGQUIT, signalHandler);

	if (DEBUG)
	{
		std::cout << GREEN << "Server started, on socket " << _socket << RESET << std::endl;
		std::cout <<  GREEN "\tListening on port " << _port << RESET <<  std::endl;
	}
	while (_running)
	{
		if (poll(&_fds[0], _fds.size(), -1) == -1 && _running)
			std::cerr << RED << "Error polling" << RESET << std::endl;
		else
		{
			if (_fds[0].revents & POLLIN)
				acceptSocket();
			for (size_t i = 1; i < _fds.size(); i++)
			{
				if (_fds[i].revents & POLLIN)
					receiveFromClient(_clients[i - 1]);
			}
		}
	}
}

int	Server::createSocket()
{
	int					i = 1;
	struct pollfd		fds;

	setHints();
	_socket = socket(_serverInfo->ai_family, _serverInfo->ai_socktype, _serverInfo->ai_protocol);
	if (_socket < 0)
		throw std::runtime_error("Error creating socket");
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(int)) < 0)
		throw std::runtime_error("Error setting socket options");
	if (fcntl(_socket, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("Error setting socket to non-blocking");
	if (bind(_socket, _serverInfo->ai_addr, _serverInfo->ai_addrlen) < 0)
		throw std::runtime_error("Error binding socket");
	if (listen(_socket, SOMAXCONN) < 0)
		throw std::runtime_error("Error listening socket");
	fds.fd = _socket;
	fds.events = POLLIN;
	fds.revents = 0;
	_fds.push_back(fds);
	return (0);
}

void	Server::setHints()
{
	_hints.ai_family = AF_INET;							// Ipv4
	_hints.ai_socktype = SOCK_STREAM;					// Use TCP stream sockets
	_hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;		// Bind to any suitable address
	if (getaddrinfo(NULL, _port.c_str(), &_hints, &_serverInfo) < 0) {
		throw std::runtime_error("Error retrieving address information");
	}
}

int Server::acceptSocket()
{
	struct sockaddr_in	addr;
	socklen_t			addrLength = sizeof(addr);
	int clientSocket = accept(_socket, (struct sockaddr *)&addr, &addrLength);
	if (clientSocket < 0) {
		std::cerr << RED << "Error accepting client" << RESET << std::endl;
		return (1);
	}
	if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) < 0) {
		std::cerr << RED << "Error setting client socket to non-blocking" << RESET << std::endl;
		return (1);
	}

	struct pollfd	newClientFD;
	newClientFD.fd = clientSocket;
	newClientFD.events = POLLIN;
	newClientFD.revents = 0;

	Client	newClient(clientSocket);

	_clients.push_back(newClient);
	_fds.push_back(newClientFD);
	if (DEBUG)
		std::cout << GREEN << "Client connected from " << newClient.getSocket() << RESET << std::endl;
	return (0);
}

int	Server::sendToClient(const std::string& message, const Client& client) const
{
	if (send(client.getSocket(), message.c_str(), message.length(), 0) < 0)
	{
		std::cerr << RED << "Error sending message" << RESET << std::endl;
		return (-1);
	}
	else
		if (DEBUG)
			std::cout << GREEN << "Sent: " << message << " to " << client.getNickname() << " socket " << client.getSocket() << RESET << std::endl;
	return (0);
}

int	Server::receiveFromClient(Client& sender)
{
	char	buffer[BUFFER_SIZE];
	bzero(buffer, BUFFER_SIZE);

	int		bytes = recv(sender.getSocket(), buffer, BUFFER_SIZE, 0);
	if (bytes > 0)
	{
		buffer[bytes] = '\0';
		std::string	bufferStr(buffer);
		if (DEBUG)
			std::cout << GREEN << "Received: " << bufferStr << " from " << sender.getNickname() << " socket " << sender.getSocket() << RESET << std::endl;

		parseCommand(bufferStr, sender);
		return (bytes);
	}
	else {
		std::cout << RED << "Client disconnected" << RESET << std::endl;
		std::string	message = "User has been disconnected";
		quit(sender, message);
	}
	return (0);
}

Client*	Server::findClient(const std::string& nick)
{
	if (DEBUG)
	{
		std::cout << "getClient: " << nick << std::endl;
		std::cout << "clients.size(): " << _clients.size() << std::endl;
	}
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (DEBUG)
			std::cout << "clients[i].getNickname(): " << _clients[i].getNickname() << std::endl;
		if (nick == _clients[i].getNickname())
			return (&_clients[i]);
	}
	return (NULL);
}

Channel*	Server::findChannel(const std::string& name)
{
	for (size_t i = 0; i < _channels.size(); i++)
	{
		if (name == _channels[i].getName())
			return (&_channels[i]);
	}
	return (NULL);
}

Client*	Server::usernameIsRegistered(const std::string& username)
{
	for (clientIt it = _clients.begin(); it != _clients.end(); ++it) {
		if (username == it->getUsername())
			return &(*it);
	}
	return NULL;
}

void	Server::closeSockets()
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (DEBUG)
			std::cout << RED << "Closing socket" << RESET << std::endl;
		close(_clients[i].getSocket());
	}
	if (_socket != -1)
	{
		if (DEBUG)
			std::cout << RED << "Closing server socket" << RESET << std::endl;
		close(_socket);
	}
}

bool	Server::_running = false;
void	Server::signalHandler(int signum)
{
	std::cout << YELLOW <<"Signal received" << signum << RESET << std::endl;
	_running = false;
}
