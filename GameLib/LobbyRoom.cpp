#include "LobbyRoom.h"

LobbyRoom::LobbyRoom() {}

LobbyRoom::LobbyRoom(std::string _roomName, std::string _password, unsigned short _numPlayers) :
	roomName(_roomName), password(_password), numPlayers(_numPlayers), turn(0){
	srand(time(NULL));
	idLobbyRoom = rand() % 60000;

	stackOfCards.push_back( Card (Card::CardType::TOOL, Card::CardName::WRENCH));
	stackOfCards.push_back( Card (Card::CardType::TOOL, Card::CardName::CANDLESTICK));
	stackOfCards.push_back( Card (Card::CardType::TOOL, Card::CardName::KNIFE));
	stackOfCards.push_back( Card (Card::CardType::TOOL, Card::CardName::LEADPIPE));
	stackOfCards.push_back( Card (Card::CardType::TOOL, Card::CardName::REVOLVER));
	stackOfCards.push_back( Card (Card::CardType::TOOL, Card::CardName::ROPE));
						    
	stackOfCards.push_back( Card (Card::CardType::CHARACTER, Card::CardName::BLANCO));
	stackOfCards.push_back( Card (Card::CardType::CHARACTER, Card::CardName::AMAPOLA));
	stackOfCards.push_back( Card (Card::CardType::CHARACTER, Card::CardName::RUBIO));
	stackOfCards.push_back( Card (Card::CardType::CHARACTER, Card::CardName::CELESTE));
	stackOfCards.push_back( Card (Card::CardType::CHARACTER, Card::CardName::MORA));
	stackOfCards.push_back( Card (Card::CardType::CHARACTER, Card::CardName::PRADO));
						    
	stackOfCards.push_back( Card (Card::CardType::ROOM, Card::CardName::INVERNADERO));
	stackOfCards.push_back( Card (Card::CardType::ROOM, Card::CardName::BIBLIOTECA));
	stackOfCards.push_back( Card (Card::CardType::ROOM, Card::CardName::COCINA));
	stackOfCards.push_back( Card (Card::CardType::ROOM, Card::CardName::COMEDOR));
	stackOfCards.push_back( Card (Card::CardType::ROOM, Card::CardName::ESTUDIO));
	stackOfCards.push_back( Card (Card::CardType::ROOM, Card::CardName::SALABILLAR));
	stackOfCards.push_back( Card (Card::CardType::ROOM, Card::CardName::SALABAILE));
	stackOfCards.push_back( Card (Card::CardType::ROOM, Card::CardName::SALON));
	stackOfCards.push_back( Card (Card::CardType::ROOM, Card::CardName::VESTIBULO));

	SetupEnvelope();
}

LobbyRoom::~LobbyRoom() {}

std::string LobbyRoom::GetRoomName()
{
	return roomName;
}

unsigned short LobbyRoom::GetNumPlayers()
{
	return numPlayers;
}

unsigned short LobbyRoom::GetIdLobbyRoom()
{
	return idLobbyRoom;
}

std::string LobbyRoom::GetPasswd()
{
	return password;
}

std::vector<PlayerInfo*> LobbyRoom::GetInfoPlayersOnRoom()
{
	return playerInfo;
}

void LobbyRoom::SendDataToOtherPlayers(PlayerInfo* _playerInfo)
{
	for (int i = 0; i < playersSocket.size(); i++) {
		sf::Packet packet;
		packet << "P_JOINED" << IsLobbyFull() << _playerInfo->GetName() << _playerInfo->GetIdColor();

		playersSocket[i]->send(packet);
	}
}

void LobbyRoom::SendMessageToOtherPlayers(std::string _nick, std::string _message) {
	for (int i = 0; i < playersSocket.size(); i++) {
		sf::Packet packet;
		packet << "MSG" << _nick << _message;

		playersSocket[i]->send(packet);
	}
}

void LobbyRoom::SendColorChangeToOtherPlayers(std::string _nick, unsigned short _newColor) {
	for (int i = 0; i < playersSocket.size(); i++) {
		sf::Packet packet;
		packet << "COLOR_RESPONSE" << _nick << _newColor;

		playersSocket[i]->send(packet);
	}
}

void LobbyRoom::SetUniqueColor(PlayerInfo* newPlayer) {
	bool colorExists = false;
	PlayerInfo::Color newRandomColor;

	do {
		newRandomColor = PlayerInfo::GetRandomColor();
		colorExists = false;
		for (PlayerInfo* pI : playerInfo) {
			if (pI->GetColor() == newRandomColor) {
				colorExists = true;
				break;
			}
		}
	} while (colorExists);

	newPlayer->SetColor(newRandomColor);
}

bool LobbyRoom::IsLobbyFull()
{
	return (playerInfo.size() + 1) == numPlayers;
}

void LobbyRoom::AddPlayer(sf::TcpSocket* newPlayer, PlayerInfo* newPlayerInfo)
{
	playersSocket.push_back(newPlayer);
	playerInfo.push_back(newPlayerInfo);
}

bool LobbyRoom::SetPlayerColor(std::string playerNick, unsigned short playerColor) {

	for (PlayerInfo* pI : playerInfo) {
		if (PlayerInfo::IdColorToColor(playerColor) == pI->GetColor())
			return false;
	}

	for (PlayerInfo* pI : playerInfo) {
		if (pI->GetName() == playerNick) {
			pI->SetColor(PlayerInfo::IdColorToColor(playerColor));
			return true;
		}
	}
	return false;
}

void LobbyRoom::NextTurn() {
	turn = (turn + 1) % playersSocket.size();
}

unsigned short LobbyRoom::RollDie() {
	return (rand() % 6) + 1;
}

bool LobbyRoom::IsPlayerTurn(sf::TcpSocket* socket) {

	if (playersSocket[turn]->getRemoteAddress() == socket->getRemoteAddress() && 
		playersSocket[turn]->getRemotePort() == socket->getRemotePort()) {
		return true;
	}

	return false;
}

Card LobbyRoom::DrawCard() {
	unsigned short randomCard = rand() % stackOfCards.size();
	Card cardToReturn = stackOfCards[randomCard];
	stackOfCards.erase(stackOfCards.begin() + randomCard);
	return cardToReturn;
}

Card LobbyRoom::DrawCard(Card::CardType _cardType) {
	unsigned short randomCard;
	Card cardToReturn;

	do {
		randomCard = rand() % stackOfCards.size();
		cardToReturn = stackOfCards[randomCard];
	} while (cardToReturn.GetType() != _cardType);

	stackOfCards.erase(stackOfCards.begin() + randomCard);
	return cardToReturn;
}

void LobbyRoom::SetupEnvelope() {
	envelopCards.push_back(DrawCard(Card::CardType::TOOL));
	envelopCards.push_back(DrawCard(Card::CardType::CHARACTER));
	envelopCards.push_back(DrawCard(Card::CardType::ROOM));

	Utils::print(std::to_string(static_cast<unsigned short>((envelopCards[0].GetName()))));
	Utils::print(std::to_string(static_cast<unsigned short>((envelopCards[1].GetName()))));
	Utils::print(std::to_string(static_cast<unsigned short>((envelopCards[2].GetName()))));
}

void LobbyRoom::SendCards()
{
	std::vector<unsigned short> numOfCardsPerStack;

	for (int i = 0; i < playersSocket.size(); i++) {
		numOfCardsPerStack.push_back(0);
	}

	for (int i = 0; i < stackOfCards.size(); i++) {
		numOfCardsPerStack[i % playersSocket.size()]++;
	}

	for (int i = 0; i < playersSocket.size(); i++) {

		sf::Packet packetWithCards;
		packetWithCards << "START" << numOfCardsPerStack[i];

		for (int j = 0; j < numOfCardsPerStack[i]; j++) {

			Card card = DrawCard();
			unsigned short typeCard = static_cast<unsigned short>(card.GetType());
			unsigned short nameCard = static_cast<unsigned short>(card.GetName());

			packetWithCards << typeCard << nameCard;
		}

		if (playersSocket[i]->send(packetWithCards) != sf::Socket::Done) {
			Utils::print("Error al enviar las cartas al usuario!");
		}
	}
}

std::map<short, std::vector<Card>> LobbyRoom::SetPeersCards()
{

	std::map<short, std::vector<Card>> cards;
	std::vector<unsigned short> numOfCardsPerStack;
	int numCards = 18;

	for (short i = 0; i < playersSocket.size(); i++) {
		numOfCardsPerStack.push_back(0);
		numOfCardsPerStack[i] = 18 / playersSocket.size();
		numCards -= 18 / playersSocket.size();
	}

	for (int i = 0; numCards > 0;i++)
	{
		numOfCardsPerStack[i]++;
		numCards--;
	}

	for (short i = 0; i < playersSocket.size(); i++) {	
		
		for (int j = 0; j < numOfCardsPerStack[i]; j++) {
			Card card = DrawCard();
			cards[i].push_back(card);
		}

	}
	std::cout << playersSocket.size() << ", " << numOfCardsPerStack.size() << ", " << cards.size() << std::endl;
	return cards;
}
