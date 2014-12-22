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

boost::asio::io_service io_service;
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

void send(action_t action, unsigned value) {
	try
	{
		std::cout << "SENDING " << sock << std::endl;
		boost::array<char, 2> send_buf = { (char)action, (char)value };
		sock->send_to(boost::asio::buffer(send_buf), server_endpoint);
		std::cout << "SENDED" << std::endl;
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}

bool connect() {
	try
	{
		std::cout << "CONNECTING" << std::endl;
		

		udp::resolver resolver(io_service);
		udp::resolver::query query("127.0.0.1", PORT);
		server_endpoint = *resolver.resolve(query);

		udp::socket socket(io_service);
		socket.open(udp::v4());
		sock = &socket;
		
		boost::array<char, 2> send_buf = { 0, 1 };
		sock->send_to(boost::asio::buffer(send_buf), server_endpoint);
		/*socket.send_to(boost::asio::buffer(send_buf), receiver_endpoint);

		boost::array<char, 128> recv_buf;
		udp::endpoint sender_endpoint;
		size_t len = socket.receive_from(
			boost::asio::buffer(recv_buf), sender_endpoint);

		std::cout.write(recv_buf.data(), len);*/
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return false;
	}
	return true;
}

int main(int argc, char* argv[])
{
	if (connect()) {
		std::cout << "CONNECTED" << std::endl;
		send(ACTION_BEGIN_GAME, 0);
	}
	else {

	}
	return 0;
}