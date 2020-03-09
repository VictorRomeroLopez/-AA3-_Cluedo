#pragma once
#include <iostream>
#include <string>
#include <vector>

#include "Utils.h"
#include "LobbyRoom.h"

class LobbyRoom;

class Console {

public:
	enum class ConsoleCommand {
		EXIT,
		LOBBYROOMS,
		NONE
	};

	static ConsoleCommand isCommand(std::string command);
	static void console(bool &running, std::vector<LobbyRoom*>& rooms);
	static void initConsole(bool &running, std::vector<LobbyRoom*>& rooms);
};