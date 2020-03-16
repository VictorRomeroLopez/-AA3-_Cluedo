#pragma once
#include <iostream>
#include <string>

class Messages
{
public:
	enum class Msg {
		LOGIN,
		JOIN,
		JOIN_RESPONSE,
		CREATE,
		CREATE_RESPONSE,
		COLOR,
		COLOR_RESPONSE,
		READY,
		MSG,
		P_JOINED,
		GAMES,
		REFRESH,

		START,
		RESOLVE,
		DADO,
		PISTA,
		CASILLA,
		MOVE,
		DEDUCCION,
		DESMENTIR,
		NONE
	};

	static Msg IsMessage(std::string);
	static Msg IsChatMessage(std::string);
};

