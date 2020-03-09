#include "Console.h"
#include <thread>

Console::ConsoleCommand Console::isCommand(std::string command) {
	if (command == "exit") return ConsoleCommand::EXIT;
	else if (command == "lobbys") return ConsoleCommand::LOBBYROOMS;
	return ConsoleCommand::NONE;

}

void Console::console(bool &running, std::vector<LobbyRoom*>& rooms) {

	ConsoleCommand command = ConsoleCommand::NONE;
	std::string inputConsole = "";

	while (command != ConsoleCommand::EXIT) {
		std::cin >> inputConsole;
		switch (isCommand(inputConsole)) {

		case ConsoleCommand::EXIT: {
			Utils::print("exit");
			running = false;
			return;
		}
		case ConsoleCommand::LOBBYROOMS: {
			//prints all lobbys
			for (int i = 0; i < rooms.size(); i++) {
				std::cout << i << ". " << rooms.at(i)->GetRoomName() << " Room size: " << rooms.at(i)->GetNumPlayers() << " Room id: " <<  rooms.at(i)->GetIdLobbyRoom() << std::endl;
			}
			break;
		}
		default:
			Utils::print("The command " + inputConsole + " does not exist!");
		}
	}
}

void Console::initConsole(bool & running, std::vector<LobbyRoom*>& rooms)
{
	std::thread consoleThread(Console::console, std::ref(running), std::ref(rooms));
	consoleThread.detach();
}
