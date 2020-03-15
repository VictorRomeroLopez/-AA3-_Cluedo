#pragma once
#include <string>
#include <vector>

class Card
{
public:
	enum class CardType {
		TOOL,
		CHARACTER,
		ROOM
	};
	enum class CardName {
		WRENCH = 0,
		CANDLESTICK = 1,
		LEADPIPE = 2,
		ROPE = 3,
		REVOLVER = 4,
		KNIFE = 5,

		CELESTE = 6,
		PRADO = 7,
		AMAPOLA = 8,
		RUBIO = 9,
		BLANCO = 10,
		MORA = 11,

		INVERNADERO = 12,
		SALABILLAR = 13,
		BIBLIOTECA = 14,
		ESTUDIO = 15,
		SALABAILE = 16,
		VESTIBULO = 17,
		COCINA = 18,
		COMEDOR = 19,
		SALON = 20
	};

private:
	CardType cardType;
	CardName cardName;

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

public:
	Card();
	Card(unsigned short, unsigned short);
	Card(CardType, CardName);
	~Card();

	CardType GetType();
	CardName GetName();
	std::string print();

};