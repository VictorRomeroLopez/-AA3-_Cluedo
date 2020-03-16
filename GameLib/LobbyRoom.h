#pragma once
#include <SFML\Network.hpp>
#include "PlayerInfo.h"
#include "Card.h"
#include "Utils.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <ctime>

class LobbyRoom
{
	std::vector<sf::TcpSocket*> playersSocket;
	std::vector<PlayerInfo*> playerInfo;
	std::vector<Card> stackOfCards;
	std::vector<Card> clueCards;
	std::vector<Card> envelopCards;
	std::string roomName;
	std::string password;
	unsigned short numPlayers;
	unsigned short idLobbyRoom;
	unsigned short turn;
	

public:
	LobbyRoom();
	LobbyRoom(std::string _roomName, std::string _password, unsigned short _numPlayers);
	~LobbyRoom();

	std::string GetRoomName();
	unsigned short GetNumPlayers();
	unsigned short GetIdLobbyRoom();
	std::string GetPasswd();
	std::vector<PlayerInfo*> GetInfoPlayersOnRoom();
	void SendDataToOtherPlayers(PlayerInfo* _playerInfo);
	void SendMessageToOtherPlayers(std::string _nick, std::string _message);
	void SendColorChangeToOtherPlayers(std::string _nick, unsigned short _newColor);
	void SetUniqueColor(PlayerInfo* playerInfo);
	bool IsLobbyFull();

	void AddPlayer(sf::TcpSocket* newPlayer, PlayerInfo* newPlayerInfo);
	bool SetPlayerColor(std::string playerNick, unsigned short playerColor);
	void NextTurn();
	static unsigned short RollDie();
	bool IsPlayerTurn(sf::TcpSocket*);
	Card DrawCard();
	Card DrawCard(Card::CardType);
	void SetupEnvelope();
	void SendCards();
	std::vector<Card> GetClueCards();
	void EraseClueCard(Card::CardName card);
	std::map<short, std::vector<Card>> SetPeersCards();
};

