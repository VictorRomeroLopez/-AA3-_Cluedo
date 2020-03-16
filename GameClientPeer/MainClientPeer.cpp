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

void ReceivesManager(PlayerInfo& _playerInfo, std::vector<Player>& players, int peerID, LobbyRoom& lobbyRoom, unsigned int random) {
	bool connectedToPeer = true;
	while (connectedToPeer) {
		sf::Packet receivePacket;
		sf::Socket::Status peerStatus;


		peerStatus = players[peerID].tcpSocket->receive(receivePacket);

		if (peerStatus == sf::Socket::Status::Disconnected) {
			Utils::print("Server has been disconected!");
			connectedToPeer = false;
		}
		else if (peerStatus != sf::Socket::Status::Done) {
			Utils::print("Something went wrong trying to receive a packet from server");
			continue;
		}

		std::string headder;

		receivePacket >> headder;

		switch (Messages::IsMessage(headder)) {
		case Messages::Msg::MSG: {
			std::string nickName;
			std::string message;

			receivePacket >> nickName >> message;
			aMensajes.push_back(nickName + "> " + message);

			if (aMensajes.size() > 25)
				aMensajes.erase(aMensajes.begin(), aMensajes.begin() + 1);

			break;
		}
		case Messages::Msg::DADO: {
			int status;
			receivePacket >> status;

			if(status !=0)
				lobbyRoom.NextTurn();

			players[peerID].info.SetDieThrow(lobbyRoom.RollDie());
			std::cout << "Dado de " << players[peerID].info.GetName() << " : " << players[peerID].info.GetDieThrow() << std::endl;
			break;
			}

		case Messages::Msg::PISTA: {
			std::string name;
			int idPlayer;
			receivePacket >> name;
			Card::CardName cardName = _playerInfo.StringToCardName(name);
			lobbyRoom.EraseClueCard(cardName);
			receivePacket >> idPlayer;

			if(idPlayer != -1)
				std::cout << "La carta " << name << " la tiene: " << players[idPlayer].info.GetName() << std::endl;
			else
				std::cout << "La carta que se ha descubierto es: " << name << ". Esta dentro del sobre! " << std::endl;

			lobbyRoom.NextTurn();

			break;
			}
								 
		}
	}
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

void GetChatCommand(std::vector<Player>& players, PlayerInfo& playerInfo, std::string _message, short myPlayerOrder, LobbyRoom& lobbyRoom, unsigned int random) {
	sf::Packet packetCommand;
	std::vector<std::string*> parameters = GetParameters(_message);
	switch (Messages::IsChatMessage(*parameters[0])) {
			
		case Messages::Msg::DADO: {
			packetCommand << "DADO";
			
			players[myPlayerOrder].info.SetDieThrow(LobbyRoom::RollDie());
			std::cout << "Te ha tocado un " << players[myPlayerOrder].info.GetDieThrow() << std::endl;

			if (players[myPlayerOrder].info.GetDieThrow() == 1)
			{
				short type = rand() % 3;
					std::cout <<type << std::endl;

				if (type == 0)
				{
					std::cout << "Pista! Escoge una carta del tipo ARMA con el comando \\clue y la carta deseada: " << std::endl;
					playerInfo.SetClueCardType(Card::CardType::TOOL);
				}
				else if (type == 1)
				{
					std::cout << "Pista! Escoge una carta del tipo PERSONAJE con el comando \\clue y la carta deseada: " << std::endl;
					playerInfo.SetClueCardType(Card::CardType::CHARACTER);
				}
				else if (type == 2)
				{
					std::cout << "Pista! Escoge una carta del tipo LUGAR con el comando \\clue y la carta deseada: " << std::endl;
					playerInfo.SetClueCardType(Card::CardType::ROOM);
				}
				packetCommand << 0;
			}
			else
			{
				lobbyRoom.NextTurn();
				packetCommand << 1;
			}

			for (int i = 0; i < players.size(); i++)
			{
				if (i != myPlayerOrder)
				{
					if (players[i].tcpSocket->send(packetCommand) == sf::Socket::Done) {
						Utils::print("se ha enviado el dado");

					}
				}
			}

			break;
		}	
		case Messages::Msg::PISTA: {
			Card::CardName name = playerInfo.StringToCardName(*parameters[1]);
			std::vector<Card> cards = lobbyRoom.GetClueCards();
			std::vector<Card> playerCards;
			int idPlayer;
			bool found = false;
			for (int i = 0; i < cards.size(); i++)
			{
				if (cards[i].GetName() == name)
				{
					if (cards[i].GetType() == playerInfo.GetClueCardType())
					{
						
						for (int j = 0; j < players.size(); j++)
						{
							playerCards = players[j].info.GetCards();
							for (int k = 0; k < playerCards.size(); k++)
							{
								if (playerCards[k].GetName() == name) {
									std::cout << "La carta " << *parameters[1] << " la tiene: " << players[j].info.GetName() << std::endl;
									found = true;
									idPlayer = j; 
									lobbyRoom.EraseClueCard(name);
									break;
								}
							}
						}
						if (!found)
						{
							std::cout << "La carta " << *parameters[1] << " esta dentro del sobre! " << std::endl;
							lobbyRoom.EraseClueCard(name);
							idPlayer = -1;
							found = true;
						}


						packetCommand << "PISTA";
						packetCommand << *parameters[1];
						packetCommand << idPlayer;

						lobbyRoom.NextTurn();

						for (int j = 0; j < players.size(); j++)
						{
							if (j != myPlayerOrder)
							{
								if (players[j].tcpSocket->send(packetCommand) == sf::Socket::Done) {
									Utils::print("se ha enviado la pista");

								}
							}
						}

					}
					else
						std::cout << "Esta carta no forma parte del tipo que te he pedido!" << std::endl;
				}
			}
			if (!found)
				Utils::print("¡Esta carta no existe o ya se ha descubierto con una pista!");
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

	LobbyRoom lobbyRoom;

	for (int i = 0; i < peers.size(); i++) {
		if (peers[i].tcpSocket != mySocket)
		{
			Utils::print("Thread");
			std::thread thread(&ReceivesManager, std::ref(playerInfo), std::ref(peers), i, std::ref(lobbyRoom), random);
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
					if (mensaje[0] == '\\') {
						Utils::print("COMMAND");
						GetChatCommand(peers, playerInfo, mensaje, myPlayerOrder, lobbyRoom, random);
					}
					else {
						Utils::print("MEssage");
						packet << "MSG" << playerInfo.GetName() << packetToSend;
						for (int i = 0; i < peers.size(); i++) {
							if (peers[i].tcpSocket != mySocket)
								peers[i].tcpSocket->send(packet);
						}
						
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
	return 0;
}