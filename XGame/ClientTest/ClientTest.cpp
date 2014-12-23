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
#include <queue>
#include <stdlib.h>
#include <vector>

#define PORT "51116"

bool close;


using boost::asio::ip::udp;
enum action_t {
	ACTION_OPEN_GAME,
	ACTION_BEGIN_GAME,
	ACTION_END_LEVEL,
	ACTION_LOSE_LIFE,
	ACTION_BUY_ITEM,
	ACTION_END_GAME,
};

struct informacio {
	action_t action;
	int value;
};


class CuaActiva
{
	std::queue<informacio>  cuaInfo;
	bool isActive;
public:
	//constructor
	CuaActiva(){
		isActive = false;
	};
	//destructor
	~CuaActiva(){
		while (!cuaInfo.empty()){
			cuaInfo.pop();
		}
	};
	//posar coses a la cua
	void putIn(informacio type){
		enter();
		cuaInfo.push(type);
		exit();
	}
	//returnar l'informacio de la cua
	informacio getFirstInfo(){
		enter();
		informacio i=cuaInfo.front();
		cuaInfo.pop();
		exit();
		return i;
	}
	//canvia semafor
	void enter(){
		while (isActive);
		isActive = true;
	}
	void exit() {
		isActive = false;
	}

	bool empty() {
		enter();
		bool empty = cuaInfo.empty();
		exit();
		return empty;
	}
};

CuaActiva queueInfo;

void connection(const char *host) {
	try {
		std::cout << "CONNECTING to " << host << std::endl;
		boost::asio::io_service io_service;
		udp::resolver resolver(io_service);
		udp::resolver::query query(host, PORT);
		udp::endpoint server_endpoint = *resolver.resolve(query);

		udp::socket socket(io_service);
		socket.open(udp::v4());

		// send init game
		while (!close || !queueInfo.empty()){
			if (!queueInfo.empty()){
				informacio i = queueInfo.getFirstInfo();
				std::cout << "SENDING" << std::endl;
				boost::array<char, 2> send_buf = { i.action, i.value%256 };
				socket.send_to(boost::asio::buffer(send_buf), server_endpoint);
			}
			else{
				std::chrono::milliseconds dura(10);
				std::this_thread::sleep_for(dura);
			}
		}

		
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}

int main(int argc, char* argv[])
{
	close = false;
	std::thread t(connection, "127.0.0.1");
	for (int i = 0; i < 20; ++i) {
		informacio info = {(action_t)(rand() % (ACTION_END_GAME + 1)), i};
		queueInfo.putIn(info);
	}
	informacio info = { ACTION_END_GAME, 256 };
	queueInfo.putIn(info);
	close = true;
	t.join();
	return 0;
}