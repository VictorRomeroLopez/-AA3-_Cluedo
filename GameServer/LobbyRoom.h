#pragma once
#include <SFML\Network.hpp>
#include <PlayerInfo.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <ctime>


class LobbyRoom
{
	std::vector<sf::TcpSocket*> playersSocket;
	std::vector<PlayerInfo*> playerInfo;
	std::string roomName;
	std::string password;
	unsigned short numPlayers;
	unsigned short idLobbyRoom;

public:
	LobbyRoom();
	LobbyRoom(std::string _roomName, std::string _password, unsigned short _numPlayers);
	~LobbyRoom();

	std::string GetRoomName();
	unsigned short GetNumPlayers();
	unsigned short GetIdLobbyRoom();
	std::string GetPasswd();
	std::vector<PlayerInfo*> GetInfoPlayersOnRoom();

	void AddPlayer(sf::TcpSocket* newPlayer, PlayerInfo* newPlayerInfo);
};

