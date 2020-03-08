#include "LobbyRoom.h"

LobbyRoom::LobbyRoom() {}

LobbyRoom::LobbyRoom(std::string _roomName, std::string _password, unsigned short _numPlayers) :
	roomName(_roomName), password(_password), numPlayers(_numPlayers) {}

LobbyRoom::~LobbyRoom() {}