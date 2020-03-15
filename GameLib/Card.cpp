#include "Card.h"

Card::Card() :cardType(CardType::TOOL), cardName(CardName::WRENCH) {}

Card::Card(unsigned short _cardType, unsigned short _cardName)
{
	cardType = (CardType) _cardType;
	cardName = (CardName) _cardName;
}

Card::Card(CardType _cardType, CardName _cardName) : cardType(_cardType), cardName(_cardName) {}

Card::~Card() {}

Card::CardType Card::GetType()
{
	return cardType;
}

Card::CardName Card::GetName()
{
	return cardName;
}

std::string Card::print()
{
	return cardNames[static_cast<unsigned short>(cardName)];
}
