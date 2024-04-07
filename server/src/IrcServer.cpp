/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kekuhne <kekuhne@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/04 13:28:32 by kekuhne           #+#    #+#             */
/*   Updated: 2024/04/07 15:25:47 by kekuhne          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../include/IrcServer.hpp"
# include "../include/Client.hpp"
#define DEBUG 1

Server::Server()
{
	//std::cout << "Default Server Consturctor called" << std::endl;
	_port = 6969;
	_ip = "127.0.0.1";
	_password = "Red Flag";
};

Server::Server(int port, std::string ip, std::string password)
{
	//std::cout << "Server Constructor called" << std::endl;
	_port = port;
	_ip = ip;
	_password = password;
};

Server::~Server()
{
	//std::cout << "Server Destructor called" << std::endl;
};

void Server::start()
{
	createSocket();
	std::cout << GRE << "Server started, on socket " << _socket << WHI << std::endl;
	std::cout <<  GRE "\tListening on port " << _port << WHI <<  std::endl;
	while (!_signal)
	{
		if (poll(&_fds[0], _fds.size(), -1) == -1 && !_signal)
			std::cerr << RED << "Error polling" << WHI << std::endl;
		else
		{
			if (_fds[0].revents & POLLIN)
				acceptSocket(_password);
			for (size_t i = 1; i < _fds.size(); i++)
			{
				if (_fds[i].revents & POLLIN)
					receiveSocket(_clients[i - 1].getSocket());
			}
		}
	}
};

//creates a socket
//AF_INET = IPv4
//SOCK_STREAM = TCP
//0 = Protocol is choosen by the system (since i set SOCK_STREAM this is kind of redundant, no clue why it's there :D)
// setsockopt is used to set the socket options, in this case we set the socket to reuse the address
//fcntl is used to set the socket to non-blocking, calls like recv() will suspend the program until data is received, non-blocking will return immediately
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
	int i = 1;
	struct pollfd fds;
	struct sockaddr_in addr;
	
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(_port);
	if ((_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw std::runtime_error("Error creating socket");
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(int)) < 0)
		throw std::runtime_error("Error setting socket options");
	if (fcntl(_socket, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("Error setting socket to non-blocking");
	if (bind(_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		throw std::runtime_error("Error binding socket");
	if (listen(_socket, SOMAXCONN) < 0)
		throw std::runtime_error("Error listening socket");
	fds.fd = _socket;
	fds.events = POLLIN;
	fds.revents = 0;
	_fds.push_back(fds);
	return (1);
};

//accepts the socket and stores the client fd in _clientFd vector
//password is not used yet, will be used for password protected server later(maybe)
//_socket = server socket we created in createSocket
// _addr = pointer to sockaddr_in struct that will store the client address
// addrlen = size of the sockaddr_in struct
int Server::acceptSocket(std::string password)
{
	(void)password;
	Client client;
	struct sockaddr_in addr;
	struct pollfd fds;

	int client_socket;
	if ((client_socket = accept(_socket, (struct sockaddr *)&addr, (socklen_t *)&addr)) < 0)
		std::cerr << RED <<  "Error accepting client" << WHI << std::endl;
	if (fcntl(client_socket, F_SETFL, O_NONBLOCK) < 0)
		std::cerr << RED << "Error setting client socket to non-blocking" << WHI << std::endl;
	fds.fd = client_socket;
	fds.events = POLLIN;
	fds.revents = 0;
	client.setSocket(client_socket);
	client.setIp(inet_ntoa(addr.sin_addr));
	_clients.push_back(client);
	_fds.push_back(fds);
	if (DEBUG)
		std::cout << GRE << "Client connected from " << client.getIp() << WHI << std::endl;
	return (0);
};

//sends a message to the client
int Server::sendSocket(std::string message, int client_socket)
{
	(void)message;
	(void)client_socket;
	return (0);
};

int Server::receiveSocket(int client_socket)
{
	(void)client_socket;
	return (0);
};

void Server::closeSocket()
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (DEBUG)
			std::cout << RED << "Closing socket" << WHI << std::endl;
		close(_clients[i].getSocket());
	}
	if (_socket != -1)
	{
		if (DEBUG)
			std::cout << RED << "Closing server socket" << WHI << std::endl;
		close(_socket);
	}
};

bool Server::_signal = false;
void Server::signalHandler(int signum)
{
	std::cout << YEL <<"Signal received" << signum << WHI << std::endl;
	_signal = true;
};