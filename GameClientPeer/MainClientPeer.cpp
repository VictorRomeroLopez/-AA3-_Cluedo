#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <atltypes.h>
#include "Graphics.h"
#include <PlayerInfo.h>
#include <Utils.h>
#include <LobbyRoom.h>
#include <Messages.h>

using namespace sf;

const std::string SERVER_IP = "127.0.0.1";
const u_short SERVER_PORT = 50000;
const u_short SERVER_WAIT_TIME = 5;
const u_short MAX_PLAYERS = 3;

std::vector<std::string> aMensajes;

struct Player {
	TcpSocket* tcpSocket;
	short order;
	PlayerInfo info;

	Player(TcpSocket* _tcpSocket, std::string _nick) :tcpSocket(_tcpSocket), info(_nick) {}
};

void print(std::string text) {
	std::cout << text << std::endl;
}

void PeerListener(Player* player, LobbyRoom& lobbyRoom) {
	
	while (true) {
		sf::Packet packet;
		player->tcpSocket->receive(packet);		
		std::string mensaje;
		packet >> mensaje;
		aMensajes.push_back(player->info.GetName() + mensaje);

		if (aMensajes.size() > 25)
		{
			aMensajes.erase(aMensajes.begin(), aMensajes.begin() + 1);
		}
		lobbyRoom.NextTurn();
	}
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
				Utils::print("Esta arribant al client el color: " + std::to_string(settedColor));
				_playerInfo.SetColor(PlayerInfo::IdColorToColor(settedColor));
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
				unsigned short settedColor;

				receivePacket >> settedColor;
				receivePacket >> startGame;

				Utils::print("Esta arribant al client el color: " + std::to_string(settedColor));
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
			break;
		}
		case Messages::Msg::P_JOINED: {
			std::string newPlayerName;
			unsigned short newPlayerColor;

			receivePacket >> startGame >> newPlayerName >> newPlayerColor;
			players.push_back(PlayerInfo(newPlayerName, newPlayerColor));
			Utils::print(newPlayerName + " has joined the room!");
		}
		case Messages::Msg::MSG: {

			std::string nickName;
			std::string message;

			receivePacket >> nickName >> message;
			aMensajes.push_back(nickName + "> " + message);

			if (aMensajes.size() > 25)
				aMensajes.erase(aMensajes.begin(), aMensajes.begin() + 1);

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
				Utils::print("Esta cambiant color");
				_playerInfo.SetColor(PlayerInfo::IdColorToColor(newPlayerColor));
			}

			break;
		}
		case Messages::Msg::DADO: {
			unsigned short dieThrow;
			receivePacket >> dieThrow;
			Utils::print(std::to_string(dieThrow));
			_playerInfo.SetDieThrow(dieThrow);
			break;
		}
		case Messages::Msg::START: {
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
		}
	}
}


int main()
{
	Vector2i screenDimensions(800, 600);
	Font font;
	RenderWindow window;
	TcpListener listener;
	TcpSocket socket;
	int playersCount;
	unsigned int random;
	std::vector<Player> peers;
	u_short portServer;
	Packet pack;
	Socket::Status serverStatus;
	sf::String mensaje;
	std::string nickname;
	PlayerInfo playerInfo;
	short order = 0;
	short myPlayerOrder;
	TcpSocket* mySocket = new TcpSocket();
	

#pragma region CONNECTION TO SERVER
	Utils::print("Connecting...");
	serverStatus = socket.connect(SERVER_IP, SERVER_PORT, sf::seconds(SERVER_WAIT_TIME));

	if (serverStatus != Socket::Status::Done) {
		print("There was a problem trying to connect to the server");
		return 0;
	}
	Utils::print("Conected!");

	std::cout << "Type your nickname:" << std::endl;
	std::cin >> nickname;

	playerInfo.SetName(nickname);

	pack << nickname;
	socket.send(pack);

	portServer = socket.getLocalPort();

	serverStatus = socket.receive(pack);
	if (serverStatus != Socket::Status::Done) {
		print("Esta reventando en el receive");
	}

	socket.disconnect();

	pack >> playersCount;

	pack >> random;

	srand(random);

	for (short i = 0; i < playersCount; i++)
	{
		std::string ip_peer;
		short port_peer;
		std::string nick_peer;
		pack >> ip_peer >> port_peer >> nick_peer;
		TcpSocket* sockAux = new TcpSocket();
		sockAux->connect(ip_peer, port_peer);
		peers.push_back(Player(sockAux, nick_peer));
		peers[i].order = order;
		order++;
		Packet nickPack;
		nickPack << nickname;
		sockAux->send(nickPack);
	}

	peers.push_back(Player(mySocket, playerInfo.GetName()));
	myPlayerOrder = order;
	order++;

	print("Waiting for other players...");
	std::cout << MAX_PLAYERS - playersCount - 1 << std::endl;

	if (listener.listen(portServer) != Socket::Status::Done)
	{
		print("error at listener");
	}

	for (int i = 0; i < MAX_PLAYERS - playersCount - 1; i++)
	{
		TcpSocket* socketToAccept = new TcpSocket();
		listener.accept(*socketToAccept);
		std::string nickpeer;
		socketToAccept->receive(pack);
		pack >> nickpeer;
		print((nickpeer + "entrered"));
		peers.push_back(Player(socketToAccept, nickpeer));
		peers[i].order = order;
		order++;
	}

	listener.close();
#pragma endregion

	window.create(sf::VideoMode(screenDimensions.x, screenDimensions.y), "Chat " + nickname);
	if (!font.loadFromFile(FONT_PATH))
	{
		std::cout << "Can't load the font file" << std::endl;
	}

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

	LobbyRoom lobbyRoom;

	for (int i = 0; i < peers.size(); i++) {
		if (peers[i].tcpSocket != mySocket)
		{
			Utils::print("Thread");
			std::thread thread(&PeerListener, &peers[i], std::ref(lobbyRoom));
			thread.detach();
			Utils::print("detach");
		}
	}
	LobbyRoom lb("Room", "1234", MAX_PLAYERS);
	lobbyRoom = lb;
	
	for (short i = 0; i < peers.size(); i++)
	{
		lobbyRoom.AddPlayer(peers[i].tcpSocket, &peers[i].info);
		lobbyRoom.SetUniqueColor(&peers[i].info);
	}

	for (int i = 0; i < peers.size(); i++)
	{
		std::cout <<"ID COLOR: " <<peers[i].info.GetIdColor() << std::endl;
	}

	std::map<short, std::vector<Card>> cardsMap;

	cardsMap = lobbyRoom.SetPeersCards();

	std::cout << "CardsMap: " << cardsMap.size() << ", " << myPlayerOrder << std::endl;
	for (short i = 0; i < cardsMap.size(); i++)
	{
		if (i == myPlayerOrder)
		{
			peers[i].info.SetCards(cardsMap[i]);
			playerInfo.SetCards(cardsMap[i]);
		}
		else
			peers[i].info.SetCards(cardsMap[i]);
	}

	std::cout << "cartes repartides" << std::endl;
	
	std::vector<Card> cards = playerInfo.GetCards();
	Utils::print("Cards: " + std::to_string(cards.size()));

	for (int i = 0; i < cards.size(); i++) {
		Utils::print(cards[i].print());
	}

	Utils::print("Is my turn? " + lobbyRoom.IsPlayerTurn(peers[myPlayerOrder].tcpSocket));

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
				else if (evento.key.code == sf::Keyboard::Return && lobbyRoom.IsPlayerTurn(peers[myPlayerOrder].tcpSocket))
				{
					sf::Packet packet;
					std::string packetToSend = mensaje;
					packet << packetToSend;
					for (int i = 0; i < peers.size(); i++) {
						if (peers[i].tcpSocket != mySocket)
							peers[i].tcpSocket->send(packet);
					}

					aMensajes.push_back(mensaje);

					if (aMensajes.size() > 25)
					{
						aMensajes.erase(aMensajes.begin(), aMensajes.begin() + 1);
					}

					mensaje = ">";

					lobbyRoom.NextTurn();
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
	return 0;
}