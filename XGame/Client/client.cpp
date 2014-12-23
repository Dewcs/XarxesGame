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
		cuaInfo.push(type);
	}
	//returnar l'informacio de la cua
	informacio getFirstInfo(){
		return cuaInfo.front();
	}
	//canvia semafor
	void change(){
		if (!isActive) isActive = true;
		else isActive = false;
	}

	bool empty() {
		return cuaInfo.empty();
	}
};

CuaActiva queueInfo;

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
		while (!close && !queueInfo.empty()){
			if (!close){
			}else{
			}
		}

		boost::array<char, 2> send_buf = { ACTION_OPEN_GAME, 0 };
		socket.send_to(boost::asio::buffer(send_buf), server_endpoint);
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}

void displayMenu() {

	std::cout << "MAIN MENU" << std::endl;
	std::cout << "1 - Begin Game" << std::endl;
	std::cout << "0 - Exit" << std::endl;
}

#define WORD_SIZE 10
#define START_LETTERS 4
#define BASIC_LIVES 3

std::string words[WORD_SIZE] = {
	"TREE",
	"RAIL",
	"APPLE",
	"SUBWAY",
	"LIFE",
	"WEAPON",
	"RANDOM",
	"FINAL",
	"BEGINNING",
	"SWORD",
};


bool endedGame(std::vector<bool> letters, std::string word) {
	for (int i = 0; i < word.size(); ++i) {
		if (!letters[word[i] - 'A']) return false;
	}
	return true;
}


void drawGame(std::vector<bool> letters, std::string word, int lives) {
	if (!endedGame(letters, word)) {
		std::cout << "CURRENT LIVES: " << lives << std::endl;
		std::cout << "USED LETTERS: ";
		for (int i = 0; i < letters.size(); ++i) {
			if (letters[i]) std::cout << char('A' + i) << " ";
		}
		std::cout << std::endl;
		std::cout << "WORD: ";
		for (int i = 0; i < word.size(); ++i) {
			if (letters[word[i] - 'A']) {
				std::cout << word[i] << " ";
			}
			else std::cout << "_ ";
		}
		std::cout << std::endl;
	}
	else {
		std::cout << "WORD WAS: " << word << std::endl;
	}
}

void startGame(int l) {
	std::cout << "BEGIN NEW GAME" << std::endl;
	int lives = BASIC_LIVES+l;
	int wid = rand()%WORD_SIZE;
	std::vector<bool> letters('Z' - 'A', false);
	std::cout << "ENTER "<<START_LETTERS<<" LETTERS" << std::endl;
	for (int i = 0; i < START_LETTERS; ++i) {
		char c;
		std::cin >> c;
		if (c >= 'A' && c <= 'Z') c = c - 'A' + 'a';
		if (c >= 'a' && c <= 'z' && !letters[c-'a']) {
			letters[c - 'a'] = true;
		}
		else {
			--i;
		}
	}
	while (lives > 0 && !endedGame(letters,words[wid])) {
		drawGame(letters, words[wid], lives);
		std::cout << "1 - Add letter (Cost 1 life)" << std::endl;
		std::cout << "2 - Solve word (Cost 1 life if word is not equal)" << std::endl;
		std::cout << "3 - Buy 1 live (0.89$)" << std::endl;
		std::cout << "4 - Buy 10 lives (6.99$)" << std::endl;
		std::cout << "5 - Buy 100 lives (49.99$)" << std::endl;
		std::cout << "0 - Exit" << std::endl;
		int order;
		std::cin >> order;
		if (order == 1) {
			char c;
			bool ok = false;
			while (!ok) {
				std::cout << "INPUT LETTER:" << std::endl;
				std::cin >> c;
				if (c >= 'A' && c <= 'Z') c = c - 'A' + 'a';
				if (c >= 'a' && c <= 'z' && !letters[c - 'a']) {
					letters[c - 'a'] = true;
					ok = true;
				}
			}
			--lives;
		}
		else if (order == 2) {
			std::string s;
			std::cout << "INPUT WORD:" << std::endl;
			std::cin >> s;
			if (s == words[wid]) {
				letters = std::vector<bool> ('Z' - 'A', true);
			}
			else --lives;
		}
		else if (order == 3) lives += 1;
		else if (order == 4) lives += 10;
		else if (order == 5) lives += 100;
		else if (order == 0) {
			lives = -1;
		}
	}
	if (lives > 0) {
		std::cout << "YOU WIN!" << std::endl;
		drawGame(letters, words[wid], lives);
		startGame(lives);
	}
	else {
		std::cout << "YOU LOSE!" << std::endl;
	}
}

int main(int argc, char* argv[])
{
	close = false;
	std::thread t(connection,"127.0.0.1");
	int order = -1;

	while (order != 0) {
		displayMenu();
		std::cin >> order;
		if (order == 1) {
			startGame(0);
		}
	}
	close = true;
	t.join();
	return 0;
}