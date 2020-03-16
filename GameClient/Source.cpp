#pragma once
#include <PlayerInfo.h>
#include <SFML\Network.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <fcntl.h>
#include "Graphics.h"
#include <LobbyRoom.h>
#include <Messages.h>

const sf::IpAddress SERVER_IP = sf::IpAddress("127.0.0.1");
const unsigned short SERVER_PORT = 50000;
const sf::Time SERVER_TIMEOUT = sf::seconds(5);

void AddMessageToChat(std::vector<std::string>& aMensajes, std::string message) {
	aMensajes.push_back(message);
	if (aMensajes.size() > 25)
		aMensajes.erase(aMensajes.begin(), aMensajes.begin() + 1);
}

std::vector<std::string*> GetParameters(std::string str)
{
	std::vector<std::string*> parameters;
	std::string word = "";

	for (auto x : str)
	{
		if (x == ' ')
		{
			parameters.push_back(new std::string(word));
			word = "";
		}
		else
		{
			word = word + x;
		}
	}

	parameters.push_back(new std::string(word));
	return parameters;
}

void GetChatCommand(sf::TcpSocket& serverSocket, PlayerInfo& playerInfo, std::string _message, std::vector<std::string>& aMensajes) {
	sf::Packet packetCommand;
	std::vector<std::string*> parameters = GetParameters(_message);

	switch (Messages::IsChatMessage(*parameters[0])) {
	case Messages::Msg::REFRESH: {

		sf::Packet refreshPacket;
		refreshPacket << "REFRESH";

		if (serverSocket.send(refreshPacket) != sf::Socket::Status::Done) {
			AddMessageToChat(aMensajes, "Error al enviar el paquete REFRESH");
		}

		break;
	}
	case Messages::Msg::COLOR: {
		unsigned short idColor;

		if (parameters.size() == 1) {
			Utils::print(std::to_string(playerInfo.GetIdColor()));
		}
		else if (PlayerInfo::ColorStringToIdColor(idColor, *parameters[1])) {
			packetCommand << "COLOR" << playerInfo.GetName() << idColor;

			if (serverSocket.send(packetCommand) != sf::Socket::Status::Done) {
				Utils::print("Error al enviar el paquet COLOR");
			}
		}
		else {
			Utils::print(*parameters[1] + " is not a valid parmeter for color!");
		}

		break;
	}
	case Messages::Msg::JOIN: {
		if (parameters.size() == 2)
		{
			if (*parameters[1] == "help") {
				AddMessageToChat(aMensajes, "\\join [roomId] [roomPwd]");
			}
			else {
				AddMessageToChat(aMensajes, "Wrong usage of the command "+ *parameters[0] + "! Check out " + *parameters[0] + " help");
				break;
			}
		}
		else if (parameters.size() == 3) {
			std::string gameId = *parameters[1];
			std::string password = *parameters[2];
			sf::Packet joinPacket;

			joinPacket << "JOIN" << gameId << password;

			if (serverSocket.send(joinPacket) != sf::Socket::Status::Done) {
				AddMessageToChat(aMensajes, "Failed to send the join request to the server");
			}
		}
		break;
	}
	case Messages::Msg::CREATE: {
		if (parameters.size() == 2)
		{
			if (*parameters[1] == "help") {
				AddMessageToChat(aMensajes, "\\create [roomName] [roomPwd] [numPlayers(3-6)]");
			}
			else {
				AddMessageToChat(aMensajes, "Wrong usage of the command " + *parameters[0] + "! Check out " + *parameters[0] + " help");
				break;
			}
		}
		else if (parameters.size() == 4) {
			sf::Packet createLobbyRoomPacket;
			std::string roomName = *parameters[1];
			std::string roomPwd = *parameters[2];
			unsigned short numPlayers;

			if (std::numeric_limits<unsigned short>::max() > std::stoi(*parameters[3])) {
				numPlayers = (unsigned short)std::stoi(*parameters[3]);
				if (numPlayers < 3 || numPlayers > 6) {
					AddMessageToChat(aMensajes, "Number of players must stay between 3 and 6! Check out \\create help");
					break;
				}
			}
			else {
				AddMessageToChat(aMensajes, "Wrong usage of the command " + *parameters[0] + "! Check out " + *parameters[0] + " help");
				break;
			}

			createLobbyRoomPacket << "CREATE" << roomName << roomPwd << numPlayers;

			if (serverSocket.send(createLobbyRoomPacket) != sf::Socket::Status::Done) {
				AddMessageToChat(aMensajes, "Failed to send the create request to the server");
			}
		}
		else {
			AddMessageToChat(aMensajes, "Wrong usage of the command " + *parameters[0] + "! Check out " + *parameters[0] + " help");
			break;
		}

		break;
	}
	case Messages::Msg::READY: {
		Utils::print("READY");
		break;
	}
	case Messages::Msg::LOGIN: {
		if (parameters.size() < 2 || parameters[1]->empty()) {
			AddMessageToChat(aMensajes, "Incorrect parameters for command \\login!");
		}
		else
		{
			sf::Packet loginPacket;
			loginPacket << "LOGIN" << *parameters[1];

			if (serverSocket.send(loginPacket) != sf::Socket::Status::Done) {
				AddMessageToChat(aMensajes, "Error at sending the login packet to the server!");
			}
			else
			{
				playerInfo.SetName(*parameters[1]);
				AddMessageToChat(aMensajes, "Username setted to " + *parameters[1]);
				AddMessageToChat(aMensajes, "Use the commands \\create or \\join to either create or join to a room!");
				AddMessageToChat(aMensajes, "For more info \\[command] help!");
			}
		}

		break;
	}
	case Messages::Msg::DADO: {
		packetCommand << "DADO";
		if (serverSocket.send(packetCommand) == sf::Socket::Done) {
			Utils::print("se ha enviado el dado");
		}
	}
	case Messages::Msg::START: {
		std::vector<Card> cards = playerInfo.GetCards();

		Utils::print(std::to_string(cards.size()));

		for (int i = 0; i < cards.size(); i++) {
			Utils::print(cards[i].print());
		}
	}
	//esto deberia ser acusacion, pero quedan 30 minutos para la entrega
	case Messages::Msg::DEDUCCION: {
		sf::Packet acusePacket;
		acusePacket << "DEDUCCION";
		if (parameters.size() == 2)
		{
			if (*parameters[1] == "help") {
				AddMessageToChat(aMensajes, "\\acuse [weapon] [character] [room]");
			}
			else {
				AddMessageToChat(aMensajes, "Wrong usage of the command " + *parameters[0] + "! Check out " + *parameters[0] + " help");
				break;
			}
		}
		else if (parameters.size() == 4) {
			acusePacket << static_cast<unsigned short>(Card::StringToCardName(*parameters[1])) << static_cast<unsigned short>(Card::StringToCardName(*parameters[2])) << static_cast<unsigned short>(Card::StringToCardName(*parameters[3]));
		}

		if (serverSocket.send(acusePacket) != sf::Socket::Status::Done) {
			//algo ha ido mal 
		}
		break;
	}
	}
}

void DrawChat(sf::TcpSocket& serverSocket, PlayerInfo& _playerInfo, std::vector<std::string>& aMensajes) {

	sf::Vector2i screenDimensions(800, 600);
	sf::RenderWindow window(sf::VideoMode(screenDimensions.x, screenDimensions.y), "Chat");
	sf::Font font;
	sf::String mensaje;
	font.loadFromFile(FONT_PATH);

	mensaje = "";

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
					if (mensaje[0] == '\\') {
						GetChatCommand(serverSocket, _playerInfo, mensaje, aMensajes);
					}
					else {
						packet << "MSG" << _playerInfo.GetName() << packetToSend;
						serverSocket.send(packet);
					}

					mensaje = "";
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

void ReceivesManager(PlayerInfo& _playerInfo, sf::TcpSocket& serverSocket, bool& startGame, std::vector<PlayerInfo>& players, std::vector<std::string>& aMensajes) {
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
				unsigned short settedColor;
				receivePacket >> settedColor;
				_playerInfo.SetColor(PlayerInfo::IdColorToColor(settedColor));
				AddMessageToChat(aMensajes, "Lobby created correctly");
			}
			else
			{
				AddMessageToChat(aMensajes, "Something went wrong trying to create a lobby");
			}

			break;
		}
		case Messages::Msg::JOIN_RESPONSE: {
			bool statusRequest;
			unsigned short numPlayersRequest;

			receivePacket >> statusRequest;

			if (statusRequest) {
				AddMessageToChat(aMensajes, "Succesfully joined to the server!");
				unsigned short settedColor;

				receivePacket >> settedColor;
				receivePacket >> startGame;

				_playerInfo.SetColor(PlayerInfo::IdColorToColor(settedColor));
				receivePacket >> numPlayersRequest;

				for (int i = 0; i < numPlayersRequest; i++) {

					std::string playerName;
					unsigned short playerColor;
					receivePacket >> playerName >> playerColor;
					players.push_back(PlayerInfo(playerName, playerColor));
					std::cout << playerName << ' ' << playerColor << std::endl;
				}
			}
			else {
				AddMessageToChat(aMensajes, "Id or pwd incorrect!");
			}
			break;
		}
		case Messages::Msg::GAMES: {
			unsigned short numGames;
			receivePacket >> numGames;

			AddMessageToChat(aMensajes, "");
			AddMessageToChat(aMensajes, "Available lobbys:");
			if (numGames == 0) {
				AddMessageToChat(aMensajes, "There isn't any lobby yet!");
			}
			else {
				for (int i = 0; i < numGames; i++) {
					std::string roomName = "";
					unsigned short roomId = 0;
					unsigned short currentPlayers = 0;
					unsigned short maxPlayers = 0;

					receivePacket >> roomName >> roomId >> currentPlayers >> maxPlayers;

					AddMessageToChat(aMensajes, std::to_string(i + 1) + 
						". Name: " + roomName + 
						" Id: " + std::to_string(roomId) + 
						" Players: " + std::to_string(currentPlayers) + '/' + std::to_string(maxPlayers));
				}
			}
			break;
		}
		case Messages::Msg::P_JOINED:{
			std::string newPlayerName;
			unsigned short newPlayerColor;

			receivePacket >> startGame >> newPlayerName >> newPlayerColor;
			players.push_back(PlayerInfo(newPlayerName, newPlayerColor));
			AddMessageToChat(aMensajes, newPlayerName + " has joined the room!");
		}
		case Messages::Msg::MSG:{

			std::string nickName;
			std::string message;

			receivePacket >> nickName >> message;
			AddMessageToChat(aMensajes, nickName + "> " + message);

			break;
		}
		case Messages::Msg::COLOR_RESPONSE: {
			std::string newPlayerNick;
			unsigned short newPlayerColor;
			receivePacket >> newPlayerNick >> newPlayerColor;

			if (newPlayerNick == "-1") break;

			for (int i = 0; i < players.size(); i++) {
				if (players[i].GetName() == newPlayerNick) {
					players[i].SetColor(PlayerInfo::IdColorToColor(newPlayerColor));
					break;
				}
			}

			if (_playerInfo.GetName() == newPlayerNick) {
				AddMessageToChat(aMensajes, "You have succesfully changed your color");
				_playerInfo.SetColor(PlayerInfo::IdColorToColor(newPlayerColor));
			}

			break;
		}
		case Messages::Msg::DADO: {
			std::string nickPlayer;
			unsigned short dieThrow;
			receivePacket >>nickPlayer>> dieThrow;

			AddMessageToChat(aMensajes, nickPlayer + " has rolled a " + std::to_string(dieThrow));

			if(nickPlayer == _playerInfo.GetName())
				_playerInfo.SetDieThrow(dieThrow);

			break;
		}
		case Messages::Msg::START:{
			unsigned short cardType;
			unsigned short cardName;
			unsigned short numCards;
			receivePacket >> numCards;
			std::vector<Card>* cardsHand = new std::vector<Card>();

			for (int i = 0; i < numCards; i++) {
				receivePacket >> cardType >> cardName;
				cardsHand->push_back(Card(cardType, cardName));
			}

			_playerInfo.SetCards(*cardsHand);
			break;
		}
		case Messages::Msg::RESOLVE: {

			bool resolve;
			std::string playerNick;
			unsigned short weaponId;
			unsigned short characterId;
			unsigned short placeId;

			receivePacket >> resolve >> playerNick;

			if (resolve) 
			{
				receivePacket >> weaponId >> characterId >> placeId;

				std::string weaponName = Card::GetCardName(weaponId);
				std::string characterName = Card::GetCardName(characterId);
				std::string placeName = Card::GetCardName(placeId);

				AddMessageToChat(aMensajes, playerNick + " solved the crime. The crime was performed on the " + placeName + 
					" whith a "+ weaponName + 
					" by " + characterName);
			}
			else 
			{
				AddMessageToChat(aMensajes, playerNick + " tried to solve the crime, but was wrong!");
			}

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
	std::vector<Card> currentCards;

	AddMessageToChat(aMensajes, "type \\login [nickname] in order to register to the server!");

	if (!ManageConnectionToServer(serverSocket)) return 1;

	std::thread receiveThread(&ReceivesManager, std::ref(playerInfo), std::ref(serverSocket), std::ref(startGame), std::ref(players), std::ref(aMensajes));
	receiveThread.detach();

	if (!startGame) {
		Utils::print("Waiting for other players...");
	}

	std::thread chatThread(&DrawChat, std::ref(serverSocket), std::ref(playerInfo), std::ref(aMensajes));

	//Mentre no son tots els jugadors a la partida
	while (!startGame) {

	}

	Graphics g;
	g.DrawDungeon(playerInfo, players);

	return 0;
}