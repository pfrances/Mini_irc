/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/06 18:25:22 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/19 10:14:51 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <cstdlib>
#include <string>

int	main(int argc, char **argv) {
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << "[ip address] [port]" << std::endl;
		return 1;
	}
	std::string ip = argv[1];
	int port = std::atoi(argv[2]);

	try {
		Client client(ip, port);
		client.run();
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
