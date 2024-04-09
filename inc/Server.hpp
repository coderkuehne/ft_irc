/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkwasny <kkwasny@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/04 13:13:15 by kekuhne           #+#    #+#             */
/*   Updated: 2024/04/09 16:15:58 by kkwasny          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include <cstring>
# include <unistd.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <exception>
# include <fcntl.h>
# include <vector>
# include <netinet/in.h>
# include <csignal>
# include <poll.h>
# include <cstdlib>

//-------------------------------------------------------//
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"
//-------------------------------------------------------//

# define BUFFER_SIZE 1024
# define PORT 6969

class Client;

class Server
{
	private:
		const std::string&	_port;
		const std::string	_password;

		int					_socket;

		std::vector<Client>			_clients;
		std::vector<struct pollfd>	_fds;
		static bool					_running;

	public:
		Server(const std::string& = "7789", const std::string& password = "123");
		~Server();

		void	start();
		int		createSocket();
		int		acceptSocket();
		void		checkPassword(Client client);
		int		sendSocket(std::string message, int client_socket);
		int		receiveSocket(int client_socket);
		void	closeSocket();
		int		verifyPassword(int client_socket, std::string password);

		static void	signalHandler(int signum);
};

#endif
