#pragma once
#include <SFML\Network.hpp>
#include <PlayerInfo.h>
#include <iostream>
#include <vector>
#include <string>

class LobbyRoom
{
	std::vector<sf::TcpSocket> playersSocket;
	std::vector<PlayerInfo> playerInfo;
	std::string roomName;
	std::string password;
	unsigned short numPlayers;

public:
	LobbyRoom();
	LobbyRoom(std::string _roomName, std::string _password, unsigned short _numPlayers);
	~LobbyRoom();

};

