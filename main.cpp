#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

#define TILE_SIZE 40.f

int main()
{
	sf::TcpSocket socket;
	std::string consoleInput;
	std::cin >> consoleInput;
	if (consoleInput == "host")
	{
		sf::TcpListener listener;
		listener.listen(45000);
		std::cout << "Waitin for connection..." << std::endl;
		if (listener.accept(socket) != sf::Socket::Done) 
			return - 1;
	}
	else
	{
		std::cout << "Try to connect..." << std::endl;
		if (socket.connect(consoleInput, 45000, sf::seconds(10)) != sf::Socket::Done) 
			return -1;
	}

	sf::RenderWindow window(sf::VideoMode(640, 480), consoleInput);

	socket.setBlocking(false);

	sf::Vector2f shapeSize(TILE_SIZE, TILE_SIZE);
	sf::RectangleShape localShape(shapeSize);
	sf::RectangleShape remoteShape(shapeSize);
	while (window.isOpen())
	{
		sf::Vector2i moveDir;
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::W)
					moveDir.y += -1;
				else if (event.key.code == sf::Keyboard::A)
					moveDir.x += -1;
				else if (event.key.code == sf::Keyboard::S)
					moveDir.y += 1;
				else if (event.key.code == sf::Keyboard::D)
					moveDir.x += -1;
				break;
			case sf::Event::Closed:
				window.close();
				break;
			}
		}
		sf::Packet packet;
		if(socket.receive(packet) == sf::Socket::Done)
		{
			sf::Vector2f pos;
			packet >> pos.x >> pos.y;
			remoteShape.setPosition(pos);

		}
		if (moveDir.x != 0 || moveDir.y != 0)
		{
			localShape.move(moveDir.x * TILE_SIZE, moveDir.y * TILE_SIZE);
			sf::Packet packet;
			packet << localShape.getPosition().x << localShape.getPosition().y;
			if (socket.send(packet) != sf::Socket::Done)
				return -1;
		}
		window.clear();
		window.draw(localShape);
		window.draw(remoteShape);
		window.display();
	}

	return 0;
}