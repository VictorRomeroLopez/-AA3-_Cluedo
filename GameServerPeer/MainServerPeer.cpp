#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <iostream>

using namespace sf;

const unsigned short MAX_PLAYERS = 5;
const unsigned short SERVER_PORT = 50000;

struct Address {
	std::string nick;
	IpAddress ipAdress;
	unsigned short port;

	Address(std::string _nick, IpAddress _ipAddress, unsigned short _port) : nick(_nick), ipAdress(_ipAddress), port(_port) {}

};

void print(std::string text) {
	std::cout << text << std::endl;
}

int main()
{
	std::vector<Address*> peer;
	TcpListener listener;

	unsigned int random = (unsigned int)rand();

	listener.listen(SERVER_PORT);

	for (short i = 0; i < MAX_PLAYERS; i++)
	{
		TcpSocket socket;
		if (listener.accept(socket) != Socket::Status::Done) {
			print("error al listener");
		}

		std::string nickname;
		Packet nickPacket;
		socket.receive(nickPacket);
		nickPacket >> nickname;

		Packet pack;
		Address* newAddress = new Address(nickname, socket.getRemoteAddress(), socket.getRemotePort());
		peer.push_back(newAddress);

		int count = peer.size() - 1;
		pack << count;
		
		pack << random;

		for (int i = 0; i < peer.size() - 1; i++) {
			pack << peer[i]->ipAdress.toString() << peer[i]->port << peer[i]->nick;
			std::cout << peer[i]->nick << std::endl;
		}

		socket.send(pack);
		socket.disconnect();
	}

	listener.close();
	return 0;
}