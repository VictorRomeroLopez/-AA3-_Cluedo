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
		Utils::print("Conection accepted on IP: " + socket->getRemoteAddress().toString());
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

void ClientLoop(sf::TcpSocket* socket, std::vector<LobbyRoom*>* rooms) {
	PlayerInfo* clientInfo = new PlayerInfo();
	sf::Socket::Status clientStatus;
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
				break;
			}
			case Messages::Msg::JOIN: {
				std::string lobbyRoomId;
				std::string lobbyRoomPasswd;
				LobbyRoom** lobbyRoomRequested = (new LobbyRoom*());

				sf::Packet answerPacket;
				bool joinResponseStatus;

				clientPacket >> lobbyRoomId >> lobbyRoomPasswd;
				answerPacket << "JOIN_RESPONSE";

				if (GetLobbyRoomWithId((unsigned short)std::stoul(lobbyRoomId, nullptr, 0), rooms, lobbyRoomRequested)) {
					if ((*lobbyRoomRequested)->GetPasswd() == lobbyRoomPasswd) {
						joinResponseStatus = true;
						answerPacket << joinResponseStatus;
						unsigned short numPlayersOnRoom = (*lobbyRoomRequested)->GetInfoPlayersOnRoom().size();
						answerPacket << numPlayersOnRoom;
						for (int i = 0; i < (*lobbyRoomRequested)->GetInfoPlayersOnRoom().size(); i++) {
							answerPacket << (*lobbyRoomRequested)->GetInfoPlayersOnRoom()[i]->GetName() << (*lobbyRoomRequested)->GetInfoPlayersOnRoom()[i]->GetIdColor();
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
				}

				break;
			}
			case Messages::Msg::CREATE: {
				std::string lobbyRoomName;
				std::string lobbyRoomPasswd;
				unsigned short lobbyRoomNumPlayers;
				clientPacket >> lobbyRoomName >> lobbyRoomPasswd >> lobbyRoomNumPlayers;
				LobbyRoom* newLobbyRoom = new LobbyRoom(lobbyRoomName, lobbyRoomPasswd, lobbyRoomNumPlayers);
				newLobbyRoom->AddPlayer(socket, clientInfo);
				rooms->push_back(newLobbyRoom);
				break;
			}
			case Messages::Msg::COLOR:
				break;
			case Messages::Msg::READY:
				break;
			case Messages::Msg::MSG:
				break;
			case Messages::Msg::REFRESH:
				break;
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
	sf::TcpSocket* socketListener = new sf::TcpSocket();
	std::vector<LobbyRoom*>* lobbyRooms = new std::vector<LobbyRoom*>();

	bool running = true;

	if (!InitListener(listener)) return 1;

	Console::initConsole(running, *lobbyRooms);

	while (running) {
		if (ManageListenerAccept(listener, socketListener)) {
			GenerateClientThread(socketListener, lobbyRooms);
		}
	}

	return 0;
}