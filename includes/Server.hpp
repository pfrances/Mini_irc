/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/08 09:30:17 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/19 10:15:21 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERV_HPP
# define SERV_HPP

# include "utils.hpp"
# include <stdexcept>
# include <vector>
# include <map>

class Server {
public:
	Server(std::string ipAddress, int port);
	Server(const Server& other);
	Server& operator=(const Server& other);
	virtual ~Server();

	void	run(void);
	void	closeClientByFd(int clientId);
	void	closeAllSockets(void);

private:
	int						port_;
	int						ipAddressInt_;
	std::string				ipAddressString_;
	std::vector<pollfd>		socketFds_;
	std::map<int, std::string>	clientsNames_;
	int						clientsCount_;
	static const int		maxClients_ = 10;
	static const int		maxEvent_ = 20;
	static const int 		serverEventsCount_ = 2;

	Server();
	void			closeClientByIndex(int clientIndex);
	void			acceptNewClient(void);
	void			checkEvents(void);

	void			broadcastMsg(int senderFd, std::string msg);
	std::string		getMsg(int socketFd);
};


class ServerException : public std::exception {
public:
	ServerException(const std::string& message);

	virtual const char* what() const throw();
	virtual ~ServerException() throw();

private:
	std::string message_;
};

class SocketCreationFailedException : public ServerException {
public:
	SocketCreationFailedException(const std::string& message);
};

class BindFailedException : public ServerException {
public:
	BindFailedException(const std::string& message);
};

class MaxClientsReachedException : public ServerException {
public:
	MaxClientsReachedException(const std::string& message);
};

class AcceptFailedException : public ServerException {
public:
	AcceptFailedException(const std::string& message);
};

class PollFailedException : public ServerException {
public:
	PollFailedException(const std::string& message);
};

class ListenFailedException : public ServerException {
public:
	ListenFailedException(const std::string& message);
};

class InvalidClientIndexException : public ServerException {
public:
	InvalidClientIndexException(const std::string& message);
};

class ClientNotFoundException : public ServerException {
public:
	ClientNotFoundException(const std::string& message);
};

class ShutdownException : public ServerException {
public:
	ShutdownException(const std::string& message);
};

class RecvFailedException : public ServerException {
public:
	RecvFailedException(const std::string& message);
};

class SendFailedException : public ServerException {
public:
	SendFailedException(const std::string& message);
};

#endif
