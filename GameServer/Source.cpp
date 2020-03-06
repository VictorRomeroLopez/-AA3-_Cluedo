#pragma once
#include <iostream>
#include <thread>
#include <string>
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <PlayerInfo.h>
#include "Console.h"
#include "Utils.h"

const std::string SERVER_IP = "127.0.0.1";
const unsigned short SERVER_PORT = 50000;

bool InitListener(sf::TcpListener& listener) {

	if (listener.listen(SERVER_PORT) != sf::Socket::Status::Done) {
		Utils::print("Something went wrong trying to set up the listener");
		return false;
	}

	Utils::print("Listener succesfully seted up!");
	return true;

}

void ManageListenerAccept(sf::TcpListener& listener, sf::TcpSocket& socket) {

	if (listener.accept(socket) != sf::Socket::Status::Done)
	{
		Utils::print("Could not accept the conection");
	}
	else
	{
		Utils::print("Conection accepted on IP: " + socket.getRemoteAddress().toString());
	}
}

int main()
{
	sf::TcpListener listener;
	sf::TcpSocket socketListener;
	bool running = true;

	if (!InitListener(listener)) return 1;

	Console::initConsole(running);

	while (running) {
		ManageListenerAccept(listener, socketListener);

	}

	return 0;
}