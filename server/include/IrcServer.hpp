/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kekuhne <kekuhne@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/04 13:13:15 by kekuhne           #+#    #+#             */
/*   Updated: 2024/04/07 12:44:22 by kekuhne          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_SERVER_HPP
# define IRC_SERVER_HPP

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
#define RED "\e[1;31m" //-> for red color
#define WHI "\e[0;37m" //-> for white color
#define GRE "\e[1;32m" //-> for green color
#define YEL "\e[1;33m" //-> for yellow color
//-------------------------------------------------------//

# define BUFFER_SIZE 1024
# define PORT 6969
# define IP "127.0.0.1"

class Client;

	class Server
	{
		private:
			int _port;
			std::string _ip;
			int _socket;
			std::string _password;
			std::vector<Client> _clients;
			std::vector<struct pollfd> _fds;
			static bool _signal;
		public:
			Server();
			Server(int port, std::string ip, std::string password);
			~Server();
			void start();
			int createSocket();
			int acceptSocket(std::string password);
			int sendSocket(std::string message, int client_socket);
			int receiveSocket(int client_socket);
			void closeSocket();
			static void signalHandler(int signum);
	};
#endif