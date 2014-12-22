//
// client.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <thread>
#include <boost/array.hpp>
#include <boost/asio.hpp>

#define PORT "51116"

using boost::asio::ip::udp;

udp::socket *sock=NULL;
udp::endpoint server_endpoint;

enum action_t {
	ACTION_OPEN_GAME,
	ACTION_BEGIN_GAME,
	ACTION_END_LEVEL,
	ACTION_LOSE_LIFE,
	ACTION_BUY_ITEM,
	ACTION_END_GAME,
};

void connection(const char *host) {
	try {
		std::cout << "CONNECTING to "<< host << std::endl;
		boost::asio::io_service io_service;

		udp::resolver resolver(io_service);
		udp::resolver::query query(host, PORT);
		udp::endpoint server_endpoint = *resolver.resolve(query);

		udp::socket socket(io_service);
		socket.open(udp::v4());

		// send init game
		boost::array<char, 2> send_buf = { ACTION_OPEN_GAME, 0 };
		socket.send_to(boost::asio::buffer(send_buf), server_endpoint);
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}

int main(int argc, char* argv[])
{
	std::thread t(connection,"127.0.0.1");

	t.join();
	return 0;
}