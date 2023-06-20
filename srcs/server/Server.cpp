/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 11:47:41 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/18 18:56:48 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(std::string ipAddress, int port) :	port_(port),
													ipAddressString_(ipAddress),
													clientsCount_(0) {
	this->socketFds_.reserve(maxClients_ + serverEventsCount_);
	this->ipAddressInt_ = stringIpToInt(ipAddress);

	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port_); // 0000 1000 -- > 8 ? 1 ? host to network short int
	serverAddr.sin_addr.s_addr = htonl(ipAddressInt_);

	pollfd serverFd;
	serverFd.fd = socket(AF_INET, SOCK_STREAM, AF_UNSPEC);
	if (serverFd.fd < 0) {
		throw SocketCreationFailedException("socket initialization failed.");
	}
	serverFd.events = POLLIN;
	serverFd.revents = 0;
	if (bind(serverFd.fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
		throw BindFailedException("bind failed");
	}
	this->socketFds_.push_back(serverFd);
	serverFd.fd = STDIN_FILENO;
	serverFd.events = POLLIN;
	serverFd.revents = 0;
	this->socketFds_.push_back(serverFd);
	this->clientsNames_[serverFd.fd] = "Admin";
}

Server::Server(const Server& other) : port_(other.port_),
								ipAddressInt_(other.ipAddressInt_),
								ipAddressString_(other.ipAddressString_),
								socketFds_(other.socketFds_),
								clientsNames_(other.clientsNames_),
								clientsCount_(other.clientsCount_) {

}

Server& Server::operator=(const Server& other) {
	if (this != &other) {
		this->closeAllSockets();
		this->port_ = other.port_;
		this->ipAddressInt_ = other.ipAddressInt_;
		this->ipAddressString_ = other.ipAddressString_;
		this->socketFds_ = other.socketFds_;
		this->clientsNames_ = other.clientsNames_;
		this->clientsCount_ = other.clientsCount_;
	}
	return *this;
}

Server::~Server() {
	this->closeAllSockets();
}

void Server::closeAllSockets(void) {
	std::vector<pollfd>::iterator it = this->socketFds_.begin();
	std::vector<pollfd>::iterator ite = this->socketFds_.end();

	for (; it != ite; it++) {
		if (it->fd != STDIN_FILENO) {
			close(it->fd);
		}
	}
}

void Server::run(void) {

	std::vector<pollfd>::iterator it = this->socketFds_.begin();

	if (listen(it->fd, maxEvent_) < 0) {
		throw ListenFailedException("listen failed");
	}

	while (1)
	{
		if (poll(this->socketFds_.data(), this->socketFds_.size(), -1) < 0) {
			throw PollFailedException("poll failed");
		}
		this->checkEvents();
	}
}

void Server::acceptNewClient(void) {

	if (this->clientsCount_ == maxClients_) {
		throw MaxClientsReachedException("max clients reached");
	}

	struct sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	int clientSocket = accept(this->socketFds_.begin()->fd, (struct sockaddr *)&clientAddr, &clientAddrLen);
	if (clientSocket == -1) {
		throw AcceptFailedException("accept failed");
	}

	pollfd newClient;
	newClient.fd = clientSocket;
	newClient.events = POLLIN;
	newClient.revents = 0;
	this->socketFds_.push_back(newClient);
	std::cout << "New client connected from: "
		<< inet_ntoa(clientAddr.sin_addr) << " "
		<< ntohs(clientAddr.sin_port) << std::endl;
	this->clientsCount_++;
}

void Server::checkEvents(void) {

	std::vector<pollfd>::iterator it = this->socketFds_.begin();
	std::vector<pollfd>::iterator ite = this->socketFds_.end();

	std::string	msg;

	if (it->revents & POLLIN) {
		this->acceptNewClient();
	}
	it++;

	if (it->revents & POLLIN) {
		std::getline(std::cin, msg);
		if (msg == "shutdown") {
			throw ShutdownException("shutdown command received");
		}
		broadcastMsg(it->fd, msg);
	}
	it++;

	for (; it != ite; it++) {
		if (it->revents & POLLIN) {
			try {
				msg = this->getMsg(it->fd);
				// if (this->clientsNames_[it->fd].empty()) {
				// 	this->clientsNames_[it->fd] = msg;
				// 	continue;
				if (msg == "shutdown") {
					throw ShutdownException("shutdown command received");
				}
				std::cout << "[" << this->clientsNames_[it->fd] << "] " << msg << std::endl;
				broadcastMsg(it->fd, msg);
			} catch (RecvFailedException& e) {
				this->closeClientByFd(it->fd);
				it--;
			}
		}
	}
}

void Server::broadcastMsg(int senderFd, std::string msg) {
	std::vector<pollfd>::iterator it = this->socketFds_.begin() + serverEventsCount_;
	std::vector<pollfd>::iterator ite = this->socketFds_.end();

	msg = "[" + this->clientsNames_[senderFd] + "] " + msg;

	for (; it != ite; it++) {
		if (it->fd != senderFd) {
			int bytesSent = send(it->fd, msg.c_str(), msg.length(), 0);
			if (bytesSent < 0) {
				throw SendFailedException("send failed");
			} else if (bytesSent < static_cast<int>(msg.length())) {
				throw SendFailedException("Incomplete message sent");
			}
		}
	}
}

void Server::closeClientByIndex(int clientIndex) {
	if (clientIndex < 0 || clientIndex > this->clientsCount_ + serverEventsCount_) {
		throw InvalidClientIndexException("Invalid client index");
	}

	std::vector<pollfd>::iterator it = this->socketFds_.begin() + clientIndex;

	std::cout << this->clientsNames_[it->fd] << " fd: " << it->fd << " disconnected." << std::endl;

	close(it->fd);
	this->socketFds_.erase(it);
	this->clientsNames_.erase(it->fd);
	this->clientsCount_--;
}

void Server::closeClientByFd(int clientFd) {
	std::vector<pollfd>::iterator it = this->socketFds_.begin() + serverEventsCount_;
	std::vector<pollfd>::iterator ite = this->socketFds_.end();
	for (; it != ite; it++) {
		if (it->fd == clientFd) {
			this->closeClientByIndex(it - this->socketFds_.begin());
			return ;
		}
	}
	throw ClientNotFoundException("Client not found");
}

std::string Server::getMsg(int socketFd) {

	char buff[BUFFER_SIZE + 1];
	std::string msg;

	while (true) {
		int bytesRead = recv(socketFd, buff, BUFFER_SIZE, 0);
		buff[bytesRead] = '\0';
		msg += buff;
		if (bytesRead < 0) {
			throw RecvFailedException("An issue occured while receiving message");
		} else if (bytesRead == 0) {
			this->closeClientByFd(socketFd);
			break ;
		} else if (bytesRead < BUFFER_SIZE) {
			break ;
		}
	}
	return msg;
}

/*										Exception										*/

ServerException::ServerException(const std::string& message) : message_(message) {

}

ServerException::~ServerException(void) throw() {

}

const char* ServerException::what(void) const throw() {
	return message_.c_str();
}

MaxClientsReachedException::MaxClientsReachedException(const std::string& message) : ServerException(message) {

}

SocketCreationFailedException::SocketCreationFailedException(const std::string& message) : ServerException(message) {

}

BindFailedException::BindFailedException(const std::string& message) : ServerException(message) {

}

AcceptFailedException::AcceptFailedException(const std::string& message) : ServerException(message) {

}

PollFailedException::PollFailedException(const std::string& message) : ServerException(message) {

}

ListenFailedException::ListenFailedException(const std::string& message) : ServerException(message) {

}

InvalidClientIndexException::InvalidClientIndexException(const std::string& message) : ServerException(message) {

}

ClientNotFoundException::ClientNotFoundException(const std::string& message) : ServerException(message) {

}

ShutdownException::ShutdownException(const std::string& message) : ServerException(message) {

}

RecvFailedException::RecvFailedException(const std::string& message) : ServerException(message) {

}

SendFailedException::SendFailedException(const std::string& message) : ServerException(message) {

}
