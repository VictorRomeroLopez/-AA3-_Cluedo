#include "Console.h"
#include <thread>

Console::ConsoleCommand Console::isCommand(std::string command) {

	if (command == "exit") return ConsoleCommand::EXIT;
	return ConsoleCommand::NONE;

}

void Console::console(bool &running) {

	ConsoleCommand command = ConsoleCommand::NONE;
	std::string inputConsole = "";

	while (command != ConsoleCommand::EXIT) {
		std::cin >> inputConsole;
		switch (isCommand(inputConsole)) {

		case ConsoleCommand::EXIT:
			Utils::print("exit");
			running = false;
			return;

		default:
			Utils::print("The command " + inputConsole + " does not exist!");
		}
	}

}

void Console::initConsole(bool & running)
{
	std::thread consoleThread(Console::console, std::ref(running));
	consoleThread.detach();
}
