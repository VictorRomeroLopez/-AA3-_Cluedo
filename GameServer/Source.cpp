#pragma once
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <thread>
#include <string>
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <Console.h>
#include "Messages.h"

class Utils;
class LobbyRoom;

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

bool ManageListenerAccept(sf::TcpListener& listener, sf::TcpSocket* socket) {

	if (listener.accept(*socket) != sf::Socket::Status::Done)
	{
		Utils::print("Could not accept the conection");
		return false;
	}
	else
	{
		Utils::print("Conection accepted on IP: " + socket->getRemoteAddress().toString() + " " + std::to_string(socket->getRemotePort()));
		return true;
	}
}

Messages::Msg GetHeadder(sf::Packet& packet) {
	std::string headder;
	packet >> headder;
	return Messages::IsMessage(headder);
}

bool GetLobbyRoomWithId(unsigned short roomId, std::vector<LobbyRoom*>* rooms, LobbyRoom** room) {
	for (int i = 0; i < rooms->size(); i++) {
		if (rooms->at(i)->GetIdLobbyRoom() == roomId) {
			*room = rooms->at(i);
			return true;
		}
	}
	return false;
}

void SendRooms(sf::TcpSocket* socket, std::vector<LobbyRoom*>* rooms) {
	sf::Packet roomsPacket;
	unsigned short roomsSize = rooms->size();
	roomsPacket << "GAMES" << roomsSize;

	if (roomsSize > 0) {
		std::string roomName;
		unsigned short roomId;
		unsigned short currentPlayers;
		unsigned short maxPlayers;

		for (int i = 0; i < roomsSize; i++) {
			roomName = rooms->at(i)->GetRoomName();
			roomId = rooms->at(i)->GetIdLobbyRoom();
			currentPlayers = rooms->at(i)->GetCurrentNumberPlayers();
			maxPlayers = rooms->at(i)->GetNumPlayers();

			roomsPacket << roomName << roomId << currentPlayers << maxPlayers;
		}
	}

	if (socket->send(roomsPacket) != sf::Socket::Status::Done) {
		Utils::print("Error trying to send GAMES packet");
	}
}

void ClientLoop(sf::TcpSocket* socket, std::vector<LobbyRoom*>* rooms) {
	PlayerInfo* clientInfo = new PlayerInfo();
	sf::Socket::Status clientStatus;
	LobbyRoom** lobbyRoomRequested = (new LobbyRoom * ());
	bool clientConnected = true;

	while (clientConnected) {
		sf::Packet clientPacket;
		clientStatus = socket->receive(clientPacket);

		if (clientStatus == sf::Socket::Status::Disconnected) {
			//TODO:	treure el jugador de tots els llocs que s'haigi de treure
			clientConnected = false;
		}

		switch (GetHeadder(clientPacket)) {
			case Messages::Msg::LOGIN: {
				std::string clientName;
				clientPacket >> clientName;
				clientInfo = new PlayerInfo(clientName);
				SendRooms(socket, rooms);
				break;
			}
			case Messages::Msg::REFRESH: {
				SendRooms(socket, rooms);
				break;
			}
			case Messages::Msg::JOIN: {
				std::string lobbyRoomId;
				std::string lobbyRoomPasswd;

				sf::Packet answerPacket;
				bool joinResponseStatus = false;
				bool isLobbyFull = false;

				clientPacket >> lobbyRoomId >> lobbyRoomPasswd;
				answerPacket << "JOIN_RESPONSE";

				if (GetLobbyRoomWithId((unsigned short)std::stoul(lobbyRoomId, nullptr, 0), rooms, lobbyRoomRequested)) {
					if ((*lobbyRoomRequested)->GetPasswd() == lobbyRoomPasswd) {

						(*lobbyRoomRequested)->SetUniqueColor(clientInfo);

						joinResponseStatus = true;
						isLobbyFull = (*lobbyRoomRequested)->IsLobbyFull();

						unsigned short numPlayersOnRoom = (*lobbyRoomRequested)->GetInfoPlayersOnRoom().size();

						answerPacket << joinResponseStatus << clientInfo->GetIdColor() << isLobbyFull << numPlayersOnRoom;

						for (int i = 0; i < (*lobbyRoomRequested)->GetInfoPlayersOnRoom().size(); i++) {
							std::string name = (*lobbyRoomRequested)->GetInfoPlayersOnRoom()[i]->GetName();
							unsigned short color = (*lobbyRoomRequested)->GetInfoPlayersOnRoom()[i]->GetIdColor();
							answerPacket << name << color;
						}
					}
					else
					{
						joinResponseStatus = false;
						answerPacket << joinResponseStatus;
					}
				}

				if (socket->send(answerPacket) != sf::Socket::Status::Done) {
					Utils::print("Something went wrong!");
					break;
				}

				if (joinResponseStatus) {
					(*lobbyRoomRequested)->SendDataToOtherPlayers(clientInfo);
					(*lobbyRoomRequested)->AddPlayer(socket, clientInfo);
					if (isLobbyFull) (*lobbyRoomRequested)->SendCards();
				}

				break;
			}
			case Messages::Msg::CREATE: {
				std::string lobbyRoomName;
				std::string lobbyRoomPasswd;
				unsigned short lobbyRoomNumPlayers;
				clientPacket >> lobbyRoomName >> lobbyRoomPasswd >> lobbyRoomNumPlayers;
				LobbyRoom* newLobbyRoom = new LobbyRoom(lobbyRoomName, lobbyRoomPasswd, lobbyRoomNumPlayers);
				lobbyRoomRequested = &newLobbyRoom;
				(*lobbyRoomRequested)->SetUniqueColor(clientInfo);
				newLobbyRoom->AddPlayer(socket, clientInfo);
				rooms->push_back(newLobbyRoom);

				sf::Packet createResponse;
				createResponse << "CREATE_RESPONSE" << true << clientInfo->GetIdColor();
				socket->send(createResponse);
				break;
			}
			case Messages::Msg::MSG: {
				std::string _nick;
				std::string _message;

				clientPacket >> _nick >> _message;
				(*lobbyRoomRequested)->SendMessageToOtherPlayers(_nick, _message);
				break;
			}
			case Messages::Msg::COLOR: {
				std::string playerNick;
				unsigned short playerColor;
				sf::Packet answerPacket;

				clientPacket >> playerNick >> playerColor;

				if ((*lobbyRoomRequested)->SetPlayerColor(playerNick, playerColor)) {
					(*lobbyRoomRequested)->SendColorChangeToOtherPlayers(playerNick, playerColor);
				}
				else {
					answerPacket << "-1";
					socket->send(answerPacket);
				}
				break;
			}
			case Messages::Msg::DADO: {
				sf::Packet dieThrow;

				if ((*lobbyRoomRequested)->IsPlayerTurn(socket)) {
					dieThrow << "DADO" << (*lobbyRoomRequested)->PlayerTurn(socket).GetName() << LobbyRoom::RollDie();
					(*lobbyRoomRequested)->SendDie(dieThrow);
				}
				break;
			}
			//De la misma forma que en el cliente esto es la acusacion pero quedan 25 minutos para la entrega
			case Messages::Msg::DEDUCCION:{
				unsigned short weaponId;
				unsigned short characterId;
				unsigned short placeId;
				bool won;

				clientPacket >> weaponId >> characterId >> placeId;

				won = (*lobbyRoomRequested)->Acusation(weaponId, characterId, placeId);

				sf::Packet resolvePacket;

				resolvePacket << won << (*lobbyRoomRequested)->PlayerTurn(socket).GetName();

				if (won)  resolvePacket << weaponId << characterId << placeId;
				
				(*lobbyRoomRequested)->SendDie(resolvePacket);
			}
		}
	}
}

void GenerateClientThread(sf::TcpSocket* socketListener, std::vector<LobbyRoom*>* rooms) {
	std::thread clientThread(&ClientLoop, socketListener, rooms);
	clientThread.detach();
}

int main()
{
	sf::TcpListener listener;
	std::vector<LobbyRoom*>* lobbyRooms = new std::vector<LobbyRoom*>();

	bool running = true;

	if (!InitListener(listener)) return 1;

	Console::initConsole(running, *lobbyRooms);

	while (running) {
		sf::TcpSocket* socketListener = new sf::TcpSocket();
		if (ManageListenerAccept(listener, socketListener)) {
			GenerateClientThread(socketListener, lobbyRooms);
		}
	}

	return 0;
}