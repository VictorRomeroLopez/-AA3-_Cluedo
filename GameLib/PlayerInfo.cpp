#include "PlayerInfo.h"
#include <random>


PlayerInfo::PlayerInfo()
{
	position.x = std::rand() % 10;
	position.y = std::rand() % 10;
	lives = 3;
	color = Color::BLUE;
}

PlayerInfo::PlayerInfo(std::string _name) :
	name(_name), position(sf::Vector2i(0,0)), lives(3), color(Color::BLUE) {}

PlayerInfo::PlayerInfo(std::string _name, unsigned short _color) : 
	name(_name), position(sf::Vector2i(0, 0)), lives(3), color(IdColorToColor(_color)) {}

PlayerInfo::~PlayerInfo()
{
}

std::string PlayerInfo::GetName()
{
	return name;
}

void PlayerInfo::SetName(std::string _name) {
	name = _name;
}

PlayerInfo::Color PlayerInfo::GetColor()
{
	return color;
}

unsigned short PlayerInfo::GetDieThrow() {
	return dieThrow;
}

void PlayerInfo::SetDieThrow(unsigned short _dieThrow) {
	dieThrow = _dieThrow;
}

sf::Vector2i PlayerInfo::GetPosition() {
	return position;
}

void PlayerInfo::SetPosition(sf::Vector2i _position) {
	position = _position;
}

PlayerInfo::Color PlayerInfo::GetRandomColor()
{
	return IdColorToColor(rand() % ColorToIdColor(Color::NONE));
}

void PlayerInfo::SetColor(PlayerInfo::Color _color) {
	color = _color;
}

unsigned short PlayerInfo::GetIdColor()
{
	return static_cast<unsigned short>(color);
}

std::vector<Card> PlayerInfo::GetCards()
{
	return hand;
}

void PlayerInfo::SetCards(std::vector<Card> _cards)
{
	hand = _cards;
}

PlayerInfo::Color PlayerInfo::IdColorToColor(unsigned short _idColor) {
	Color _color = Color::RED;

	if (_idColor == 0) _color = Color::RED;
	else if (_idColor == 1) _color = Color::GREEN;
	else if (_idColor == 2) _color = Color::BLUE;
	else if (_idColor == 3) _color = Color::YELLOW;
	else if (_idColor == 4) _color = Color::PURPLE;
	else if (_idColor == 5) _color = Color::ORANGE;
	
	return _color;
}

unsigned short PlayerInfo::ColorToIdColor(Color _color)
{
	return static_cast<unsigned short>(_color);
}

bool PlayerInfo::ColorStringToIdColor(unsigned short& _idColor, std::string colorString) {
	
	if (colorString == "red") _idColor = 0;
	else if (colorString == "green") _idColor = 1;
	else if (colorString == "blue") _idColor = 2;
	else if (colorString == "yellow") _idColor = 3;
	else if (colorString == "purple") _idColor = 4;
	else if (colorString == "orange") _idColor = 5;
	else return false;
	return true;

}

Card::CardType PlayerInfo::GetClueCardType()
{
	return clueCardType;
}

void PlayerInfo::SetClueCardType(Card::CardType clueCard)
{
	clueCardType = clueCard;
}

Card::CardName PlayerInfo::StringToCardName(std::string cardName)
{
	if (cardName == "WRENCH") return Card::CardName::WRENCH;
	else if (cardName == "CANDLESTICK") return Card::CardName::CANDLESTICK;
	else if (cardName == "LEADPIPE") return Card::CardName::LEADPIPE;
	else if (cardName == "ROPE") return Card::CardName::ROPE;
	else if (cardName == "REVOLVER") return Card::CardName::REVOLVER;
	else if (cardName == "KNIFE") return Card::CardName::KNIFE;
	else if (cardName == "CELESTE") return Card::CardName::CELESTE;
	else if (cardName == "AMAPOLA") return Card::CardName::AMAPOLA;
	else if (cardName == "RUBIO") return Card::CardName::RUBIO;
	else if (cardName == "BLANCO") return Card::CardName::BLANCO;
	else if (cardName == "MORA") return Card::CardName::MORA;
	else if (cardName == "PRADO") return Card::CardName::PRADO;
	else if (cardName == "INVERNADERO") return Card::CardName::INVERNADERO;
	else if (cardName == "SALABILLAR") return Card::CardName::SALABILLAR;
	else if (cardName == "BIBLIOTECA") return Card::CardName::BIBLIOTECA;
	else if (cardName == "ESTUDIO") return Card::CardName::ESTUDIO;
	else if (cardName == "SALABAILE") return Card::CardName::SALABAILE;
	else if (cardName == "VESTIBULO") return Card::CardName::VESTIBULO;
	else if (cardName == "COCINA") return Card::CardName::COCINA;
	else if (cardName == "COMEDOR") return Card::CardName::COMEDOR;
	else if (cardName == "SALON") return Card::CardName::SALON;
	else
		return (Card::CardName)21;
}
