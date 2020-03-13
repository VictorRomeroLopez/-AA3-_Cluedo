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

void DrawChat(std::vector<std::string>& aMensajes) {

	sf::Vector2i screenDimensions(800, 600);
	sf::RenderWindow window(sf::VideoMode(screenDimensions.x, screenDimensions.y), "Chat");
	sf::Font font;
	sf::String mensaje;
	font.loadFromFile(FONT_PATH);

	mensaje = ">";

	sf::Text chattingText(mensaje, font, 14);
	chattingText.setFillColor(sf::Color(0, 160, 0));
	chattingText.setStyle(sf::Text::Bold);

	sf::Text text(mensaje, font, 14);
	text.setFillColor(sf::Color(0, 160, 0));
	text.setStyle(sf::Text::Bold);
	text.setPosition(0, 560);

	sf::RectangleShape separator(sf::Vector2f(800, 5));
	separator.setFillColor(sf::Color(200, 200, 200, 255));
	separator.setPosition(0, 550);

	//Aqui va el listener per els missatges, pero com tenim tots els receives al client ho implementarem alla 

	while (window.isOpen())
	{
		sf::Event evento;
		while (window.pollEvent(evento))
		{
			switch (evento.type) {
			case sf::Event::Closed:
				window.close();
				break;

			case sf::Event::KeyPressed:
				if (evento.key.code == sf::Keyboard::Escape) {
					window.close();
				}
				else if (evento.key.code == sf::Keyboard::Return)
				{
					sf::Packet packet;
					std::string packetToSend = mensaje;
					packet << packetToSend;

					//send packet to server

					aMensajes.push_back(mensaje);

					if (aMensajes.size() > 25)
					{
						aMensajes.erase(aMensajes.begin(), aMensajes.begin() + 1);
					}

					mensaje = ">";
				}
				break;

			case sf::Event::TextEntered:
				if (evento.text.unicode >= 32 && evento.text.unicode <= 126)
					mensaje += (char)evento.text.unicode;
				else if (evento.text.unicode == 8 && mensaje.getSize() > 0)
					mensaje.erase(mensaje.getSize() - 1, mensaje.getSize());
				break;
			}
		}

		window.draw(separator);

		for (size_t i = 0; i < aMensajes.size(); i++)
		{
			std::string chatting = aMensajes[i];
			chattingText.setPosition(sf::Vector2f(0, 20 * i));
			chattingText.setString(chatting);
			window.draw(chattingText);
		}

		std::string mensaje_ = mensaje + "_";
		text.setString(mensaje_);
		window.draw(text);

		window.display();
		window.clear();
	}
}

bool ManageConnectionToServer(sf::TcpSocket& serverSocket) {

	if (serverSocket.connect(SERVER_IP, SERVER_PORT, SERVER_TIMEOUT) != sf::Socket::Status::Done) {
		Utils::print("Something went wrong trying to connect to server");
		return false;
	}
	Utils::print("Succesfully connected to server!");
	return true;
}

void ReceivesManager(sf::TcpSocket& serverSocket, bool& startGame, std::vector<PlayerInfo>& players) {
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
				Utils::print("Something went wrong trying to create a lobby");
			}
			break;
		}
		case Messages::Msg::JOIN_RESPONSE: {

			bool statusRequest;
			unsigned short numPlayersRequest;

			receivePacket >> statusRequest;

			if (statusRequest) {
				receivePacket >> startGame;
				receivePacket >> numPlayersRequest;

				for (int i = 0; i < numPlayersRequest; i++) {

					std::string playerName;
					unsigned short playerColor;
					receivePacket >> playerName >> playerColor;
					std::cout << playerName << ' ' << playerColor << std::endl;
				}
			}
			break;
		}
		case Messages::Msg::P_JOINED:{
			std::string newPlayerName;
			unsigned short newPlayerColor;

			receivePacket >> startGame >> newPlayerName >> newPlayerColor;
			players.push_back(PlayerInfo(newPlayerName, newPlayerColor));
			Utils::print(newPlayerName + " has joined the room!");
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
	bool startGame = false;
	unsigned short maxPlayers;
	unsigned short currentPlayersJoined;
	std::vector<PlayerInfo> players;
	std::vector<std::string> aMensajes;

	std::string nick;
	std::cout << "Type your nickname: ";
	std::cin >> nick;
	loginPacket << "LOGIN" << nick;

	if (!ManageConnectionToServer(serverSocket)) return 1;

	std::thread receiveThread(&ReceivesManager, std::ref(serverSocket), std::ref(startGame), std::ref(players));
	receiveThread.detach();

	serverSocket.send(loginPacket);

	char answer;
	do {
		std::cout << "Would you like to Join a room or Create a new one? (J/C)" << std::endl;
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

	std::thread chatThread(&DrawChat, std::ref(aMensajes));

	//Mentre no son tots els jugadors a la partida
	while (!startGame) {

	}

	Graphics g;
	g.DrawDungeon();

	return 0;
}