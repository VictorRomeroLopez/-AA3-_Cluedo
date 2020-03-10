#pragma once
#include <PlayerInfo.h>
#include <SFML\Network.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <fcntl.h>
#include "Graphics.h"
#include "Utils.h"
#include <LobbyRoom.h>
#include <Messages.h>

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

void ReceivesManager(sf::TcpSocket& serverSocket) {
	bool connectedToServer = true;

	while (connectedToServer) {
		sf::Packet receivePacket;
		sf::Socket::Status serverStatus;

		serverStatus = serverSocket.receive(receivePacket);

		if (serverStatus == sf::Socket::Status::Disconnected) {
			Utils::print("Server has been disconected!");
			connectedToServer = false;
		}
		else if (serverStatus != sf::Socket::Status::Done) {
			Utils::print("Something went wrong trying to receive a packet from server");
			continue;
		}

		std::string headder;

		receivePacket >> headder;

		switch (Messages::IsMessage(headder)) {
		case Messages::Msg::CREATE_RESPONSE: {
			bool createResponse;
			receivePacket >> createResponse;
			if (createResponse)
			{
				Utils::print("Lobby Created correctly");
			}
			else
			{
				Utils::print("Lobby Created correctly");
			}
			break;
		}
		}

	}
}

bool IsCorrectAnswerRoom(char answer) {
	return answer == 'j' || answer == 'J' || answer == 'c' || answer == 'C';
}

int main()
{
	PlayerInfo playerInfo;
	sf::TcpSocket serverSocket;
	sf::Packet loginPacket;

	if (!ManageConnectionToServer(serverSocket)) return 1;

	std::string nick;
	std::cout << "Type your nickname: ";
	std::cin >> nick;
	loginPacket << "LOGIN" << nick;
	std::thread receiveThread(&ReceivesManager, std::ref(serverSocket));
	receiveThread.detach();

	serverSocket.send(loginPacket);

	char answer;
	do {
		std::cout << "Would you like to Join a room or Create a new one? (J/C)" << std::endl;
		std::cin >> answer;
	} while (!IsCorrectAnswerRoom(answer));

	unsigned short maxPlayers;
	unsigned short currentPlayersJoined;
	std::vector<PlayerInfo> players;
	bool startGame = false;

	if (answer == 'j' || answer == 'J'){
		std::string gameId;
		std::string password;
		sf::Packet joinPacket;
		sf::Packet joinPacketRequest;
		std::string headderRequest;
		bool statusRequest;
		unsigned short numPlayersRequest;

		std::cout << "Which room would you like to join?" << std::endl;
		std::cin >> gameId;
		std::cout << "Password: ";
		std::cin >> password;
		joinPacket << "JOIN" << gameId << password;
		serverSocket.send(joinPacket);
		
		if (serverSocket.receive(joinPacketRequest) != sf::Socket::Status::Done) {
			Utils::print("Something's not working");
		}

		joinPacketRequest >> headderRequest;
		joinPacketRequest >> statusRequest;

		if (statusRequest) {
			joinPacketRequest >> startGame;
			joinPacketRequest >> numPlayersRequest;

			for (int i = 0; i < numPlayersRequest; i++) {

				std::string playerName;
				unsigned short playerColor;
				joinPacketRequest >> playerName >> playerColor;
				std::cout << playerName << ' ' << playerColor << std::endl;
			}
		}
	}
	else if (answer == 'c' || answer == 'C') 
	{
		std::string lobbyRoomName;
		std::string lobbyRoomPasswd;
		unsigned short numPlayers;

		do {
			std::cout << "Lobby name: ";
			std::cin >> lobbyRoomName;
		} while (lobbyRoomName.empty());

		do {
			std::cout << "Lobby passwd: ";
			std::cin >> lobbyRoomPasswd;
		} while (lobbyRoomPasswd.empty());

		do {
			std::cout << "Num players: ";
			std::cin >> numPlayers;
		} while (numPlayers < 3 || numPlayers > 6);

		sf::Packet createLobbyRoomPacket;
		createLobbyRoomPacket << "CREATE" << lobbyRoomName << lobbyRoomPasswd << numPlayers;
		serverSocket.send(createLobbyRoomPacket);
	}

	if (!startGame) {
		Utils::print("Waiting for other players...");
	}

	//Mentre no son tots els jugadors a la partida
	while (!startGame) {

		sf::Packet playerJoinPacket;
		std::string headder;
		std::string newPlayerName;
		unsigned short newPlayerColor;

		if (serverSocket.receive(playerJoinPacket) != sf::Socket::Status::Done) {
			Utils::print("Something went wrong trying to receive the packet!");
			continue;
		}

		playerJoinPacket >> headder >> startGame >> newPlayerName >> newPlayerColor;
		players.push_back(PlayerInfo(newPlayerName, newPlayerColor));
		Utils::print(newPlayerName + " has joined the room!");
	}

	Graphics g;
	g.DrawDungeon();

	return 0;
}