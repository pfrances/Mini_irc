/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/08 09:26:29 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/16 18:12:26 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

int	stringIpToInt(std::string ip) {
	std::string::iterator it = ip.begin();
	std::string::iterator ite = ip.end();
	int result = 0;
	int bit = 0;
	for (; it != ite; it++) {
		if (*it == '.') {
			result = (result << 8) | bit;
			bit = 0;
		} else {
			bit = bit * 10 + (*it - '0');
		}
	}
	result = (result << 8) | bit;
	return result;
}

struct sockaddr_in*	createIpAddressV4(std::string ipv4, int port) {
	struct sockaddr_in *addr = new struct sockaddr_in;
	if (addr == NULL) {
		return NULL;
	}
	addr->sin_port = htons(port);
	addr->sin_family = AF_INET;
	if (ipv4 == ANY) {
		addr->sin_addr.s_addr = INADDR_ANY;
	} else {
		addr->sin_addr.s_addr = htonl(stringIpToInt(ipv4));
	}
	return addr;
}
