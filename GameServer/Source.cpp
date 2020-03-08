#pragma once
#include <iostream>
#include <vector>
#include <thread>
#include <string>
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <PlayerInfo.h>
#include "Console.h"
#include "Utils.h"
#include "LobbyRoom.h"
#include "Messages.h"

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

void ClientLoop(sf::TcpSocket* socket, std::vector<LobbyRoom>* rooms) {
	PlayerInfo clientInfo;
	sf::Socket::Status clientStatus;
	Utils::print("Client Loop");
	bool clientConnected = true;
	//TODO: cambiar la condicion del while

	while (clientConnected) {
		Utils::print("Inside client Loop");
		sf::Packet clientPacket;
		clientStatus = socket->receive(clientPacket);

		if (clientStatus == sf::Socket::Status::Disconnected) {
			clientConnected = false;
		}

		switch (GetHeadder(clientPacket)) {
			case Messages::Msg::LOGIN: {
				std::string clientName;
				clientPacket >> clientName;
				clientInfo = PlayerInfo(clientName);
				break;
			}
			case Messages::Msg::JOIN:
				break;
			case Messages::Msg::CREATE:
				break;
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

void GenerateClientThread(sf::TcpSocket* socketListener, std::vector<LobbyRoom>* rooms) {
	std::thread clientThread(&ClientLoop, socketListener, rooms);
	clientThread.detach();
}

int main()
{
	sf::TcpListener listener;
	sf::TcpSocket* socketListener = new sf::TcpSocket();
	std::vector<LobbyRoom>* lobbyRooms = new std::vector<LobbyRoom>();

	bool running = true;

	if (!InitListener(listener)) return 1;

	Console::initConsole(running);

	while (running) {
		if (ManageListenerAccept(listener, socketListener)) {
			GenerateClientThread(socketListener, lobbyRooms);
		}
	}

	return 0;
}