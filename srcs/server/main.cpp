/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/06 18:25:22 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/16 15:21:03 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <cstdlib>

int main(int argc, char **argv) {

	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " [ip address] [port]" << std::endl;
		return 1;
	}
	try {
		Server server(argv[1], std::atoi(argv[2]));
		server.run();
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 2;
	}
	return 0;
}
