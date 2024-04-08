/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkwasny <kkwasny@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/04 13:28:32 by kekuhne           #+#    #+#             */
/*   Updated: 2024/04/08 20:49:05 by kkwasny          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#define DEBUG 1

Server::Server(const std::string& port, const std::string& password): _port(port), _password(password)
{
	_running = false;
}

Server::~Server()
{}

int Server::verifyPassword(int client_socket, std::string password)
{
	//send password request to client
	(void)client_socket;
	(void)password;
	return (0);
}

int Server::checkPassword()
{
	//getPassword
	char buffer[1024] = {0};
    int valread = read(_clients[_clients.size() - 1].getSocket(), buffer, sizeof(buffer));
    if (valread < 0)
		std::cerr << RED << "Unable to read from socket" << RESET << std::endl;

    std::string receivedPassword(buffer);

    // Check if the received password is correct
    if (receivedPassword == _password)
		std::cerr << GREEN << "Password is correct. Access granted" << RESET << std::endl;
    else
        std::cerr << RED << "Incorrect password" << RESET << std::endl;
}

void Server::start()
{
	createSocket();
	std::cout << GREEN << "Server started, on socket " << _socket << RESET << std::endl;
	std::cout <<  GREEN "\tListening on port " << _port << RESET <<  std::endl;
	_running = true;
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
				checkPassword();
				
			}
			for (size_t i = 1; i < _fds.size(); i++)
			{
				if (_fds[i].revents & POLLIN)
					receiveSocket(_clients[i - 1].getSocket());
			}
		}
	}
}

//creates a socket
//AF_INET = IPv4
//SOCK_STREAM = TCP
//0 = Protocol is choosen by the system (since i set SOCK_STREAM this is kind of redundant, no clue why it's there :D)
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
	struct sockaddr_in	addr;

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(static_cast<uint16_t>(std::atoi(_port.c_str())));
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
}

int Server::acceptSocket()
{
	Client client;
	struct sockaddr_in addr;
	struct pollfd fds;

	int client_socket;
	if ((client_socket = accept(_socket, (struct sockaddr *)&addr, (socklen_t *)&addr)) < 0)
		std::cerr << RED <<  "Error accepting client" << RESET << std::endl;
	if (fcntl(client_socket, F_SETFL, O_NONBLOCK) < 0)
		std::cerr << RED << "Error setting client socket to non-blocking" << RESET << std::endl;
	fds.fd = client_socket;
	fds.events = POLLIN;
	fds.revents = 0;
	client.setSocket(client_socket);
	client.setIp(inet_ntoa(addr.sin_addr));
	_clients.push_back(client);
	_fds.push_back(fds);
	if (DEBUG)
		std::cout << GREEN << "Client connected from " << client.getIp() << RESET << std::endl;
	return (0);
}

//sends a message to the client
int Server::sendSocket(std::string message, int client_socket)
{
	if (send(client_socket, message.c_str(), message.length(), 0) < 0)
	{
		std::cerr << RED << "Error sending message" << RESET << std::endl;
		return (-1);
	}
	else
		if (DEBUG)
			std::cout << GREEN << "Sent: " << message << RESET << std::endl;
	return (0);
}

int Server::receiveSocket(int client_socket)
{
	char buffer[1024];
	int bytes;

	if ((bytes = recv(client_socket, buffer, BUFFER_SIZE, 0)) <= 0)
	{
		std::cout << RED << "Client disconnected" << RESET << std::endl;
		for (size_t i = 0; i < _clients.size(); i++)
		{
			if (_clients[i].getSocket() == client_socket)
			{
				_clients.erase(_clients.begin() + i);
				_fds.erase(_fds.begin() + i + 1);
			}
		}
		close(client_socket);
		return (0);
	}
	else
	{
		buffer[bytes] = '\0';
		if (DEBUG)
			std::cout << GREEN << "Received: " << buffer << RESET << std::endl;
	}
	return (bytes);
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
