#pragma once
#include <SFML\Graphics.hpp>
#include "Card.h"
#include <vector>

class PlayerInfo
{
	enum class Color;

	std::string name;
	sf::Vector2i position;
	Color color;
	int lives;
	unsigned short dieThrow;
	std::vector<Card> hand;

public:

	enum class Color {
		RED,
		GREEN,
		BLUE,
		YELLOW,
		PURPLE,
		ORANGE,
		NONE
	};

	PlayerInfo();
	PlayerInfo(std::string);
	PlayerInfo(std::string, unsigned short);
	~PlayerInfo();

	std::string GetName();
	void SetName(std::string _name);
	Color GetColor();
	unsigned short GetDieThrow();
	void SetDieThrow(unsigned short);
	sf::Vector2i GetPosition();
	void SetPosition(sf::Vector2i _position);
	void SetColor(Color _color);
	unsigned short GetIdColor();
	std::vector<Card> GetCards();
	void SetCards(std::vector<Card> _cards);
	static Color GetRandomColor();
	static Color IdColorToColor(unsigned short);
	static unsigned short ColorToIdColor(Color);
	static bool PlayerInfo::ColorStringToIdColor(unsigned short& _idColor, std::string colorString);

};