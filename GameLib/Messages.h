#pragma once
#include <iostream>
#include <string>

class Messages
{
public:
	enum class Msg {
		LOGIN,
		JOIN,
		OK_JOIN,
		KO_JOIN,
		CREATE,
		OK_CREATE,
		KO_CREATE,
		COLOR,
		COLOR_SELECTED,
		KO_COLOR,
		READY,
		MSG,
		P_JOINED,
		GAMES,
		REFRESH,

		START,
		RESOLVE,
		DADO,
		CASILLA,
		MOVE,
		DEDUCCION,
		DESMENTIR,
		NONE
	};

	static Msg IsMessage(std::string);
};

