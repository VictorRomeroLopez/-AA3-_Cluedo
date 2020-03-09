#include "LobbyRoom.h"

LobbyRoom::LobbyRoom() {}

//TODO: les idLobbyRoom encara no son randoms
LobbyRoom::LobbyRoom(std::string _roomName, std::string _password, unsigned short _numPlayers) :
	roomName(_roomName), password(_password), numPlayers(_numPlayers) {
	srand(time(NULL));
	idLobbyRoom = rand() % 60000;
}

LobbyRoom::~LobbyRoom() {}

std::string LobbyRoom::GetRoomName()
{
	return roomName;
}

unsigned short LobbyRoom::GetNumPlayers()
{
	return numPlayers;
}

unsigned short LobbyRoom::GetIdLobbyRoom()
{
	return idLobbyRoom;
}

std::string LobbyRoom::GetPasswd()
{
	return password;
}

std::vector<PlayerInfo*> LobbyRoom::GetInfoPlayersOnRoom()
{
	return playerInfo;
}

void LobbyRoom::AddPlayer(sf::TcpSocket* newPlayer, PlayerInfo* newPlayerInfo)
{
	playersSocket.push_back(newPlayer);
	playerInfo.push_back(newPlayerInfo);
}
