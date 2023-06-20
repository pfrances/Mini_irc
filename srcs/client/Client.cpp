/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 14:30:50 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/16 18:14:31 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <cstring>

Client::Client(std::string ipAddress, int port) :	port_(port),
													ipAddressString_(ipAddress) {
	this->clientFd_ = socket(AF_INET, SOCK_STREAM, AF_UNSPEC);
	if (this->clientFd_ < 0) {
		throw std::runtime_error("socket initialization failed.");
	}
	this->addr = createIpAddressV4(this->ipAddressString_, this->port_);
	if (this->addr == NULL) {
		throw std::runtime_error("sockaddr_in allocation failed");
	}
	if (connect(this->clientFd_, reinterpret_cast<sockaddr*>(this->addr), sizeof(*(this->addr))) < 0) {
		throw std::runtime_error("connection failed");
	}
	std::cout << "Enter your name: ";
	std::getline(std::cin, this->loginName_);
	this->sendMsgToServ(this->loginName_);
}

Client::Client(const Client& other) : port_(other.port_),
									ipAddressString_(other.ipAddressString_),
									clientFd_(other.clientFd_),
									addr(new struct sockaddr_in),
									loginName_(other.loginName_) {
	std::memcpy(addr, other.addr, sizeof(struct sockaddr_in));
}

Client& Client::operator=(const Client& other) {
	if (this != &other) {
		this->port_ = other.port_;
		this->ipAddressString_ = other.ipAddressString_;
		this->clientFd_ = other.clientFd_;
		delete this->addr;
		this->addr = new struct sockaddr_in;
		std::memcpy(addr, other.addr, sizeof(struct sockaddr_in));
		this->loginName_ = other.loginName_;
	}
	return *this;
}

Client::~Client() {
	delete this->addr;
	close(this->clientFd_);
}

void Client::sendMsgToServ(std::string msg) {
	int bytesSent = send(this->clientFd_, msg.c_str(), msg.length(), 0);
	if (bytesSent < 0) {
		throw std::runtime_error("send failed");
	} else if (bytesSent < static_cast<int>(msg.length())) {
		throw std::runtime_error("Incomplete msg sent");
	}
}

void Client::run(void) {

	std::string msg;
	pollfd fds[2];

	fds[0].fd = STDIN_FILENO;
	fds[0].events = POLLIN;
	fds[0].revents = 0;

	fds[1].fd = clientFd_;
	fds[1].events = POLLIN;
	fds[1].revents = 0;

	while (true) {
		int result = poll(fds, 2, -1);
		if (result < 0) {
			throw std::runtime_error("poll() error");
		} else if (result == 0) {
			throw std::runtime_error("Server has shut down");
		}

		if (fds[0].revents & POLLIN) {
			std::getline(std::cin, msg);
			if (msg == "exit") {
				break;
			}
			sendMsgToServ(msg);
		}

		if (fds[1].revents & POLLIN) {
			std::cout << getMsg(this->clientFd_) << std::endl;
		}
	}
}

std::string Client::getMsg(int socketFd) {

	char buff[BUFFER_SIZE + 1];
	std::string msg;

	while (true) {
		int bytesRead = recv(socketFd, buff, BUFFER_SIZE, 0);
		buff[bytesRead] = '\0';
		msg += buff;
		if (bytesRead < 0) {
			throw std::runtime_error("An issue occured while receiving message");
		} else if (msg.length() == 0) {
			throw std::runtime_error("Server disconnected");
		} else if (bytesRead < BUFFER_SIZE) {
			break ;
		}
	}
	return msg;
}
