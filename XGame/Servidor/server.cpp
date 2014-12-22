//
// async_udp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <queue>
#include <string>
#include <sstream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <unordered_map>
#include <fstream>

#define PORT 51116
#define FNAME "log.txt"

using boost::asio::ip::udp;

enum action_t {
	ACTION_OPEN_GAME,
	ACTION_BEGIN_GAME,
	ACTION_END_LEVEL,
	ACTION_LOSE_LIFE,
	ACTION_BUY_ITEM,
	ACTION_END_GAME,
};

std::string action2str(action_t action) {
	switch (action)
	{
	case ACTION_OPEN_GAME:
		return "OPEN_GAME";
		break;
	case ACTION_BEGIN_GAME:
		return "BEGIN_GAME";
		break;
	case ACTION_END_LEVEL:
		return "END_LEVEL";
		break;
	case ACTION_LOSE_LIFE:
		return "LOSE_LIFE";
		break;
	case ACTION_BUY_ITEM:
		return "BUY_ITEM";
		break;
	case ACTION_END_GAME:
		return "END_GAME";
		break;
	default:
		break;
	}
}

struct informacio {
	enum action_t action;
	unsigned value;
};

typedef std::unordered_map<std::string, std::queue<informacio> > infomap;

class server
{
public:
	server(boost::asio::io_service& io_service, short port)
		: io_service_(io_service),
		socket_(io_service, udp::endpoint(udp::v4(), port))
	{
		socket_.async_receive_from(
			boost::asio::buffer(data_, max_length), sender_endpoint_,
			boost::bind(&server::handle_receive_from, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}

	void handle_receive_from(const boost::system::error_code& error,
		size_t bytes_recvd)
	{
		if (!error && bytes_recvd > 0)
		{
			informacio info = { ACTION_OPEN_GAME, 0 };
			for (int i = 0; i < bytes_recvd; ++i) {
				if (i == 0) info.action = (action_t)data_[i];
				else if (i < 5) {
					info.value |= (action_t)data_[i]<<8*(i-1);
				}
			}
			std::stringstream ss;
			ss << sender_endpoint_;
			std::string key = ss.str();
			userinfo[key].push(info);
			if (info.action == ACTION_END_GAME) {
				store2log(key,userinfo[key]);
			}
			dumpMem();

			// wait for next
			socket_.async_receive_from(
				boost::asio::buffer(data_, max_length), sender_endpoint_,
				boost::bind(&server::handle_receive_from, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		}
		else
		{
			std::cout << "2" << std::endl;
			socket_.async_receive_from(
				boost::asio::buffer(data_, max_length), sender_endpoint_,
				boost::bind(&server::handle_receive_from, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		}
	}

	void dumpMem() {
		std::cout << "################### DUMP #########################" << std::endl;
		for (auto& x : userinfo) {
			std::cout <<"user " << x.first << std::endl;
			for (int i = 0; i < x.second.size(); ++i) {
				informacio info = x.second.front();
				std::cout << info.action << " " << info.value << std::endl;
				x.second.pop();
				x.second.push(info);
			}
		}
		std::cout << "################### END DUMP #####################" << std::endl;
	}

	void store2log(std::string user, std::queue<informacio> q) {
		std::fstream fs;
		fs.open(FNAME, std::fstream::out | std::fstream::app);
		while (!q.empty()) {
			informacio top = q.front();
			fs << "User: " << user << ", Action: " << action2str(top.action) << ", Value: " << top.value << std::endl;
			q.pop();
		}
		fs.close();
	}

	void handle_send_to(const boost::system::error_code& error, size_t bytes_sent)
	{
		/*socket_.async_receive_from(
			boost::asio::buffer(data_, max_length), sender_endpoint_,
			boost::bind(&server::handle_receive_from, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));*/
	}

private:
	boost::asio::io_service& io_service_;
	udp::socket socket_;
	udp::endpoint sender_endpoint_;
	enum { max_length = 1024 };
	char data_[max_length];
	infomap userinfo;
};

int main(int argc, char* argv[])
{
	try {
		boost::asio::io_service io_service;
		server s(io_service, PORT);
		std::cout << "BEGIN SERVER" << std::endl;
		io_service.run();
		std::cout << "END SERVER" << std::endl;
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}