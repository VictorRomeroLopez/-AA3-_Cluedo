#include "LobbyRoom.h"

LobbyRoom::LobbyRoom() {}

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

void LobbyRoom::SendDataToOtherPlayers(PlayerInfo* _playerInfo)
{
	for (int i = 0; i < playersSocket.size(); i++) {
		sf::Packet packet;
		packet << "P_JOINED" << IsLobbyFull() << _playerInfo->GetName() << _playerInfo->GetIdColor();

		playersSocket[i]->send(packet);
	}
}

void LobbyRoom::SendMessageToOtherPlayers(std::string _nick, std::string _message) {
	for (int i = 0; i < playersSocket.size(); i++) {
		sf::Packet packet;
		packet << "MSG" << _nick << _message;

		playersSocket[i]->send(packet);
	}
}

bool LobbyRoom::IsLobbyFull()
{
	return (playerInfo.size() + 1) == numPlayers;
}

void LobbyRoom::AddPlayer(sf::TcpSocket* newPlayer, PlayerInfo* newPlayerInfo)
{
	playersSocket.push_back(newPlayer);
	playerInfo.push_back(newPlayerInfo);
}
