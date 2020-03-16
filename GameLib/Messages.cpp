#include "Messages.h"

Messages::Msg Messages::IsMessage(std::string message)
{
	if (message == "LOGIN") return Messages::Msg::LOGIN;
	else if (message == "JOIN") return Messages::Msg::JOIN;
	else if (message == "JOIN_RESPONSE") return Messages::Msg::JOIN_RESPONSE;
	else if (message == "CREATE") return Messages::Msg::CREATE;
	else if (message == "CREATE_RESPONSE") return Messages::Msg::CREATE_RESPONSE;
	else if (message == "COLOR") return Messages::Msg::COLOR;
	else if (message == "COLOR_RESPONSE") return Messages::Msg::COLOR_RESPONSE;
	else if (message == "READY") return Messages::Msg::READY;
	else if (message == "MSG") return Messages::Msg::MSG;
	else if (message == "P_JOINED") return Messages::Msg::P_JOINED;
	else if (message == "GAMES") return Messages::Msg::GAMES;
	else if (message == "REFRESH") return Messages::Msg::REFRESH;

	else if (message == "START") return Messages::Msg::START;
	else if (message == "RESOLVE") return Messages::Msg::RESOLVE;
	else if (message == "DADO") return Messages::Msg::DADO;
	else if (message == "PISTA") return Messages::Msg::PISTA;
	else if (message == "CASILLA") return Messages::Msg::CASILLA;
	else if (message == "MOVE") return Messages::Msg::MOVE;
	else if (message == "DEDUCCION") return Messages::Msg::DEDUCCION;
	else if (message == "DESMENTIR") return Messages::Msg::DESMENTIR;
	else return Messages::Msg::NONE;
}

Messages::Msg Messages::IsChatMessage(std::string message) {
	if (message == "\\login") return Messages::Msg::LOGIN;
	else if (message == "\\color") return Messages::Msg::COLOR;
	else if (message == "\\join") return Messages::Msg::JOIN;
	else if (message == "\\create") return Messages::Msg::CREATE;
	else if (message == "\\refresh") return Messages::Msg::REFRESH;
	else if (message == "\\ready") return Messages::Msg::READY;
	else if (message == "\\roll") return Messages::Msg::DADO;
	else if (message == "\\clue") return Messages::Msg::PISTA;
	else if (message == "\\cards") return Messages::Msg::START;
	else return Messages::Msg::NONE;
}
