#include "Server.hpp"
#include "Client.hpp"
#define DEBUG 1

Server::Server(const std::string& port, const std::string& password): _port(port), _password(password)
{
	_running = false;
	memset(&_hints, 0, sizeof(_hints));
}

Server::~Server()
{
	closeSocket();
	std::cout << "Shutting down" << std::endl;
}

void Server::checkPassword(std::string client_data)
{
	size_t passPos = client_data.find("PASS ");
	size_t nickPos = client_data.find("\r\nNICK");

	std::string receivedPassword;
	
	if (passPos != std::string::npos && nickPos != std::string::npos)
	{
		receivedPassword = client_data.substr(passPos + 5, nickPos - passPos - 5);
	}
	else
	    std::cerr << RED << "No password" << RESET << std::endl;
	std::cout << receivedPassword << std::endl;
    if (receivedPassword == _password)
		std::cout << GREEN << "Password is correct. Access granted" << RESET << std::endl;
    else
        std::cerr << RED << "Incorrect password" << RESET << std::endl;
}

void Server::parseOther(std::string client_data)
{
	size_t nickPos = client_data.find("\r\nNICK ");
	size_t userPos = client_data.find("\r\nUSER ");
	size_t afterUserPos = client_data.find("0 *");
	
	std::string receivedNick;
	
	if (nickPos != std::string::npos && userPos != std::string::npos)
	{
		receivedNick = client_data.substr(nickPos + 7, userPos - nickPos - 6);
	}
	std::cout << receivedNick << std::endl;

	std::string receivedUser;
	
	if (userPos != std::string::npos && afterUserPos != std::string::npos)
	{
		receivedUser = client_data.substr(userPos + 7, afterUserPos - userPos - 7);
	}
	std::cout << receivedUser << std::endl;
}

void Server::start()
{
	std::cout << GREEN << "Server starting" << RESET << std::endl;
	createSocket();
	_running = true;
	signal(SIGINT, signalHandler);
	signal(SIGQUIT, signalHandler);

//	std::cout << GREEN << "Server started, on socket " << _socket << RESET << std::endl;
//	std::cout <<  GREEN "\tListening on port " << _port << RESET <<  std::endl;

	std::cout << GREEN << "Listening..." << RESET << std::endl;
	while (_running)
	{
		if (poll(&_fds[0], _fds.size(), -1) == -1 && _running)
			std::cerr << RED << "Error polling" << RESET << std::endl;
		else
		{
			if (_fds[0].revents & POLLIN)
			{
				acceptSocket();
				/* while (!verifyPassword(_clients[_clients.size() - 1].getSocket(), _password))
					std::cerr << RED << "Incorrect password" << RESET << std::endl; */
				//suitable place to get initial data from client				
			}
			for (size_t i = 1; i < _fds.size(); i++)
			{
				if (_fds[i].revents & POLLIN)
					receiveFromClient(_clients[i - 1]);
			}
		}
	}
}

//creates a socket
// setsockopt is used to set the socket options, in this case we set the socket to reuse the address
//fcntl is used to set the socket to non-blocking, calls like recv() will suspend the program until data is received, non-blocking will return immediately if no data is available
//binds the socket to the address and port
//listens to the socket
//SOMAXCONN = maximum number of connections(128)
//fds = pollfd struct that will store the socket and events
//fds.fd = socket
//fds.events = POLLIN = data can be read
//fds.revents = 0 = no r resent events
//_fd.push_back(fds) = adds the fds struct to the vector
int Server::createSocket()
{
	int					i = 1;
	struct pollfd		fds;
//	struct sockaddr_in	addr;

//	addr.sin_family = AF_INET;
//	addr.sin_addr.s_addr = INADDR_ANY;
//	addr.sin_port = htons(static_cast<uint16_t>(std::atoi(_port.c_str())));
	setHints();
	_socket = socket(_serverInfo->ai_family, _serverInfo->ai_socktype, _serverInfo->ai_protocol);
//	if ((_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
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

void	Server::setHints(void) {
	_hints.ai_family = AF_INET;                       // Ipv4
	_hints.ai_socktype = SOCK_STREAM;                 // Use TCP stream sockets
	_hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;     // Bind to any suitable address
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
		return 1;
	}
	if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) < 0) {
		std::cerr << RED << "Error setting client socket to non-blocking" << RESET << std::endl;
		return 1;
	}

	struct pollfd	newClientFD;
	newClientFD.fd = clientSocket;
	newClientFD.events = POLLIN;
	newClientFD.revents = 0;

	Client	newClient(clientSocket);
//	client.setSocket(clientSocket);
//	client.setIp(inet_ntoa(addr.sin_addr));
	_clients.push_back(newClient);
	_fds.push_back(newClientFD);
//	if (DEBUG)
//		std::cout << GREEN << "Client connected from " << client.getIp() << RESET << std::endl;
	return 0;
}

//sends a message to the client
int Server::sendToClient(std::string message, Client client)
{
	if (send(client.getSocket(), message.c_str(), message.length(), 0) < 0)
	{
		std::cerr << RED << "Error sending message" << RESET << std::endl;
		return (-1);
	}
	else
		if (DEBUG)
			std::cout << GREEN << "Sent: " << message << RESET << std::endl;
	return (0);
}

int Server::receiveFromClient(Client sender)
{
	char	buffer[BUFFER_SIZE];
	bzero(buffer, BUFFER_SIZE);

	int		bytes = recv(sender.getSocket(), buffer, BUFFER_SIZE, 0);
	if (bytes > 0) {
		buffer[bytes] = '\0';
		if (DEBUG)
			std::cout << GREEN << "Received: " << buffer << RESET << std::endl;

		std::string	bufferStr(buffer);
		if (!sender.isRegistered()) {
			registerClient(sender, bufferStr);
			return bytes;
		}
//		sendToClient(":server PRIVMSG 42bober :Pssst this is a message\r\n", sender);
		//		handleClientRequest();
		return bytes;
	}

	std::cout << RED << "Client disconnected" << RESET << std::endl;
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].getSocket() == sender.getSocket())
		{
			_clients.erase(_clients.begin() + i);
			_fds.erase(_fds.begin() + i + 1);
		}
	}
	close(sender.getSocket());
	return (0);
}

void	Server::registerClient(Client& client, std::string& buffer)
{
	std::string	nick;
	std::string	username;
	size_t		delimiter;
//	std::string	password;
	std::string::size_type	pos;

	std::cout << "Buffer: " << buffer << std::endl;
	if ((pos = buffer.find("NICK")) != std::string::npos)
	{
		delimiter = buffer.find("\r\n");
		if (delimiter == std::string::npos)
			return;
		nick = buffer.substr(pos + 5, buffer.find(" "));
		if (nick.empty()) {
			sendToClient(":server 431 * :No nickname given\r\n", client);
			return;
		}
		if (getClient(nick) != NULL) {
			sendToClient(":server 433 * " + nick + " :Nickname is already in use\r\n", client);
			return;
		}
		client.setNickname(nick);
	}
	if ((pos = buffer.find("USER")) != std::string::npos)
	{
		if (!client.getUsername().empty()) {
			sendToClient(":server 462 * :You may not re-register\r\n", client);
			return;
		}
		delimiter = buffer.find(" ", pos + 5);
		if (delimiter == std::string::npos)
			return;
		username = buffer.substr(pos + 5, buffer.find(" ") - pos - 5);
		if (username.empty()) {
			sendToClient(":server 461 * USER :Not enough parameters\r\n", client);
			return;
		}
		if (checkClientRegistered(username) != NULL) {
			sendToClient(":server 462 * :Username in use\r\n", client);
			return;
		}
		client.setUsername(username);
	}
	client.beRegistered();
	std::cerr << "Nickname: " << client.getNickname() << std::endl;
	std::cerr << "Username: " << client.getUsername() << std::endl;
	sendToClient(":server 001 " + client.getNickname() + " :Welcome to the server, " + client.getNickname() + "\r\n", client);
//	if ((pos = buffer.find("PASS")) != std::string::npos)
//	{
//		password = buffer.substr(pos + 5, buffer.find("\r\n") - pos - 5);
//		if (password == _password)
//			client.register();
//	}
}

Client*	Server::getClient(const std::string& nick) {
	for (clientIt it = _clients.begin(); it != _clients.end(); ++it) {
		if (nick == it->getNickname())
			return &(*it);
	}
	return NULL;
}

Client*	Server::checkClientRegistered(const std::string& username) {
	for (clientIt it = _clients.begin(); it != _clients.end(); ++it) {
		if (username == it->getUsername())
			return &(*it);
	}
	return NULL;
}

void Server::closeSocket()
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

bool Server::_running = false;
void Server::signalHandler(int signum)
{
	std::cout << YELLOW <<"Signal received" << signum << RESET << std::endl;
	_running = false;
}
