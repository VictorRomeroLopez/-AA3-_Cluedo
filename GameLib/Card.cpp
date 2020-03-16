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

std::string Card::GetCardName(unsigned short cardId) {
	//Me estaba dando errores y me quedaban 10 minutos
	std::vector<std::string> cardNames{
		"WRENCH",
		"CANDLESTICK",
		"LEADPIPE",
		"ROPE" ,
		"REVOLVER",
		"KNIFE",

		"CELESTE",
		"PRADO",
		"AMAPOLA",
		"RUBIO",
		"BLANCO",
		"MORA",

		"INVERNADERO",
		"SALABILLAR",
		"BIBLIOTECA",
		"ESTUDIO",
		"SALABAILE",
		"VESTIBULO",
		"COCINA",
		"COMEDOR",
		"SALON",
	};

	return cardNames[cardId];
}

Card::CardName Card::StringToCardName(std::string cardName)
{
	if (cardName == "WRENCH") return CardName::WRENCH;
	else if (cardName == "CANDLESTICK") return CardName::CANDLESTICK;
	else if (cardName == "LEADPIPE") return CardName::LEADPIPE;
	else if (cardName == "ROPE") return CardName::ROPE;
	else if (cardName == "REVOLVER") return CardName::REVOLVER;
	else if (cardName == "KNIFE") return CardName::KNIFE;
	else if (cardName == "CELESTE") return CardName::CELESTE;
	else if (cardName == "AMAPOLA") return CardName::AMAPOLA;
	else if (cardName == "RUBIO") return CardName::RUBIO;
	else if (cardName == "BLANCO") return CardName::BLANCO;
	else if (cardName == "MORA") return CardName::MORA;
	else if (cardName == "PRADO") return CardName::PRADO;
	else if (cardName == "INVERNADERO") return CardName::INVERNADERO;
	else if (cardName == "SALABILLAR") return CardName::SALABILLAR;
	else if (cardName == "BIBLIOTECA") return CardName::BIBLIOTECA;
	else if (cardName == "ESTUDIO") return CardName::ESTUDIO;
	else if (cardName == "SALABAILE") return CardName::SALABAILE;
	else if (cardName == "VESTIBULO") return CardName::VESTIBULO;
	else if (cardName == "COCINA") return CardName::COCINA;
	else if (cardName == "COMEDOR") return CardName::COMEDOR;
	else if (cardName == "SALON") return CardName::SALON;
	else return (CardName)21;
}
