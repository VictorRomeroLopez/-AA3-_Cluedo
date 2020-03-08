#include "PlayerInfo.h"
#include <random>


PlayerInfo::PlayerInfo()
{
	position.x = std::rand() % 10;
	position.y = std::rand() % 10;
	lives = 3;
}

PlayerInfo::PlayerInfo(std::string _name) :
name(_name), position(sf::Vector2i(0,0)), lives(3){}

PlayerInfo::~PlayerInfo()
{
}
