#include "Messages.h"

Messages::Msg Messages::IsMessage(std::string message)
{
	if (message == "LOGIN") return Messages::Msg::LOGIN;
	else if (message == "JOIN") return Messages::Msg::JOIN;
	else if (message == "OK_JOIN") return Messages::Msg::OK_JOIN;
	else if (message == "KO_JOIN") return Messages::Msg::KO_JOIN;
	else if (message == "CREATE") return Messages::Msg::CREATE;
	else if (message == "OK_CREATE") return Messages::Msg::OK_CREATE;
	else if (message == "KO_CREATE") return Messages::Msg::KO_CREATE;
	else if (message == "COLOR") return Messages::Msg::COLOR;
	else if (message == "COLOR_SELECTED") return Messages::Msg::COLOR_SELECTED;
	else if (message == "KO_COLOR") return Messages::Msg::KO_COLOR;
	else if (message == "READY") return Messages::Msg::READY;
	else if (message == "MSG") return Messages::Msg::MSG;
	else if (message == "P_JOINED") return Messages::Msg::P_JOINED;
	else if (message == "GAMES") return Messages::Msg::GAMES;
	else if (message == "REFRESH") return Messages::Msg::REFRESH;
	else if (message == "START") return Messages::Msg::START;
	else if (message == "RESOLVE") return Messages::Msg::RESOLVE;
	else if (message == "DADO") return Messages::Msg::DADO;
	else if (message == "CASILLA") return Messages::Msg::CASILLA;
	else if (message == "MOVE") return Messages::Msg::MOVE;
	else if (message == "DEDUCCION") return Messages::Msg::DEDUCCION;
	else if (message == "DESMENTIR") return Messages::Msg::DESMENTIR;
	else return Messages::Msg::NONE;
}
