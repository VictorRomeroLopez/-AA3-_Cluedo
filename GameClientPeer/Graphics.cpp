#include "Graphics.h"
#include <iostream>

Graphics::Graphics()
{
	Sala invernadero("Invernadero", 0, 0, 8, 10, sf::Color::Yellow);
	Sala salaBillar("Sala de billar", 12, 0, 6, 10, sf::Color::Red);
	Sala biblioteca("Biblioteca", 22, 0, 6, 10, sf::Color::Cyan);
	Sala estudio("Estudio", 32, 0, 8, 10, sf::Color::Magenta);
	Sala salaBaile("Sala de baile", 0, 12, 8, 6, sf::Color::Blue);
	Sala vestibulo("vestibulo", 30, 12, 10, 8, sf::Color::White);
	Sala cocina("cocina", 0, 20, 10, 10, sf::Color::Green);
	Sala comedor("comedor", 13, 20, 13, 10, sf::Color(150, 0, 150));
	Sala salon("salon", 30, 22, 10, 8, sf::Color(0, 150, 150));

	salas[0] = invernadero;
	salas[1] = salaBillar;
	salas[2] = biblioteca;
	salas[3] = estudio;
	salas[4] = salaBaile;
	salas[5] = vestibulo;
	salas[6] = cocina;
	salas[7] = comedor;
	salas[8] = salon;

	centroMensajes.color = sf::Color(150, 150, 150);
	centroMensajes.origen.x = 12;
	centroMensajes.origen.y = 12;
	centroMensajes.longitud.x = 14;
	centroMensajes.longitud.y = 6;
}

sf::Color ParsePlayerColor(PlayerInfo::Color _color) {
	if (_color == PlayerInfo::Color::RED) return sf::Color::Red;
	else if (_color == PlayerInfo::Color::GREEN) return sf::Color::Green;
	else if (_color == PlayerInfo::Color::BLUE) return sf::Color::Blue;
	else if (_color == PlayerInfo::Color::YELLOW) return sf::Color::Yellow;
	else if (_color == PlayerInfo::Color::PURPLE) return sf::Color(203, 66, 245, 255);
	else if (_color == PlayerInfo::Color::ORANGE) return sf::Color(247, 157, 30, 255);
	else return sf::Color::White;
}

void Graphics::DrawDungeon(PlayerInfo& currentPlayer, std::vector<PlayerInfo>& players)
{
	sf::RenderWindow _window(sf::VideoMode(800, 600), "Cluedo");
	sf::RectangleShape shape(sf::Vector2f(SIZE, SIZE));
	shape.setOutlineColor(sf::Color::Black);
	shape.setOutlineThickness(2.f);

	while (_window.isOpen())
	{
		sf::Event event;
		bool playerMoved = false;
		while (_window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				_window.close();
				break;
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Escape)
				{
					_window.close();
				}
				if (event.key.code == sf::Keyboard::Left)
				{
					std::cout << "LEFT\n";
				}
				else if (event.key.code == sf::Keyboard::Up)
				{
					std::cout << "UP\n";
				}
				else if (event.key.code == sf::Keyboard::Right)
				{
					std::cout << "RIGTH\n";
				}
				else if (event.key.code == sf::Keyboard::Down)
				{
					std::cout << "DOWN\n";
				}
				break;
			case sf::Event::MouseButtonPressed:
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
					std::cout << (sf::Mouse::getPosition().x - _window.getPosition().x - 8) << ' ' << (sf::Mouse::getPosition().y - _window.getPosition().y - 31) << std::endl;
					currentPlayer.SetPosition(
						sf::Vector2i(
						(sf::Mouse::getPosition().x - _window.getPosition().x - 8) / SIZE,
							(sf::Mouse::getPosition().y - _window.getPosition().y - 31) / SIZE
						)
					);
				}
				break;
			}
		}

		_window.clear();

		for (int i = 0; i < W_WINDOW_TITLE; i++)
		{
			for (int j = 0; j < H_WINDOW_TITLE; j++)
			{

				shape.setFillColor(sf::Color(90, 90, 90, 255));

				shape.setPosition(sf::Vector2f(i * SIZE, j * SIZE));
				_window.draw(shape);
			}
		}

		for (size_t i = 0; i < salas.size(); i++)
		{
			salas[i].Draw(_window);

		}
		centroMensajes.Draw(_window);

		sf::Vector2i position;

		position = currentPlayer.GetPosition();
		shape.setFillColor(ParsePlayerColor(currentPlayer.GetColor()));
		shape.setPosition(sf::Vector2f(position.x * SIZE, position.y * SIZE));
		_window.draw(shape);

		for (PlayerInfo pI : players) {
			position = pI.GetPosition();
			shape.setFillColor(ParsePlayerColor(pI.GetColor()));
			shape.setPosition(sf::Vector2f(position.x * SIZE, position.y * SIZE));
			_window.draw(shape);
		}

		_window.display();
	}
}


Graphics::~Graphics()
{
}
