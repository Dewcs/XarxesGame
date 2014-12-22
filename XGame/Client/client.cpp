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
#include <boost/array.hpp>
#include <boost/asio.hpp>

#define PORT "51116"

using boost::asio::ip::udp;

int main(int argc, char* argv[])
{
	try
	{

		boost::asio::io_service io_service;

		udp::resolver resolver(io_service);
		udp::resolver::query query("127.0.0.1", PORT);
		udp::endpoint receiver_endpoint = *resolver.resolve(query);

		udp::socket socket(io_service);
		socket.open(udp::v4());

		boost::array<char, 2> send_buf = { 'a','b' };
		socket.send_to(boost::asio::buffer(send_buf), receiver_endpoint);
		socket.send_to(boost::asio::buffer(send_buf), receiver_endpoint);

		boost::array<char, 128> recv_buf;
		udp::endpoint sender_endpoint;
		size_t len = socket.receive_from(
			boost::asio::buffer(recv_buf), sender_endpoint);

		std::cout.write(recv_buf.data(), len);
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}