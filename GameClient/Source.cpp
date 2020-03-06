#pragma once
#include <PlayerInfo.h>
#include <SFML\Network.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <fcntl.h>
#include "Graphics.h"
#include "Utils.h"

const sf::IpAddress SERVER_IP = sf::IpAddress("127.0.0.1");
const unsigned short SERVER_PORT = 50000;
const sf::Time SERVER_TIMEOUT = sf::seconds(5);

bool ManageConnectionToServer(sf::TcpSocket& serverSocket) {

	if (serverSocket.connect(SERVER_IP, SERVER_PORT, SERVER_TIMEOUT) != sf::Socket::Status::Done) {
		Utils::print("Something went wrong trying to connect to server");
		return false;
	}
	Utils::print("Succesfully connected to server!");
	return true;
}

int main()
{
	PlayerInfo playerInfo;
	sf::TcpSocket serverSocket;

	if (!ManageConnectionToServer(serverSocket)) return 1;

	Graphics g;
	g.DrawDungeon();

	return 0;
}