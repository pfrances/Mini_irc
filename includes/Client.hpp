/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/08 09:29:51 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/19 10:15:23 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "utils.hpp"
# include <vector>

class Client {
public:
	Client(std::string ipAddress, int port);
	Client(const Client& other);
	Client& operator=(const Client& other);
	virtual ~Client();

	void	run(void);

private:
	int						port_;
	std::string				ipAddressString_;
	int						clientFd_;
	struct sockaddr_in		*addr;

	std::string				loginName_;

	Client();
	void			sendMsgToServ(std::string message);
	std::string		getMsg(int socketFd);
};

#endif
