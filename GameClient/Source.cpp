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
	serverSocket.send(loginPacket);

	char answer;
	do {
		std::cout << "Would you like to Join a room or create a new one? (J/C)" << std::endl;
		std::cin >> answer;
	} while (!IsCorrectAnswerRoom(answer));

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
		
		serverSocket.receive(joinPacketRequest);
		joinPacketRequest >> headderRequest;
		joinPacketRequest >> statusRequest;

		if (statusRequest) {
			joinPacketRequest >> numPlayersRequest;
			for (int i = 0; i < numPlayersRequest; i++) {
				std::string playerName;
				unsigned short playerColor;
				joinPacket >> playerName >> playerColor;
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

	Graphics g;
	g.DrawDungeon();

	return 0;
}