/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/08 10:46:30 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/19 10:15:15 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <iostream>
# include <exception>
# include <unistd.h>
# include <poll.h>

# define BUFFER_SIZE 1024
# define ANY "any"

struct sockaddr_in*	createIpAddressV4(std::string ipv4, int port);

int	stringIpToInt(std::string ip);

#endif
