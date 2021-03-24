#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <vector>

#include "NetworkClient.h"

using namespace sf;
using namespace std;


class Player
{
	Text t;
	Sprite body, netGhost;
	bool possesed = false;
public:
	string name;

	Player(bool possesed = false):possesed(possesed) {};
	void load(Texture& texture, Font& font, string playerName)
	{
		body.setTexture(texture);
		if (!possesed) body.setColor(Color::Red);
		netGhost.setTexture(texture);
		name = playerName;

		t.setFont(font);
		t.setString(name);
		//t.setCharacterSize(24);
		t.setFillColor(sf::Color::Red);
	};
	void setPosition(Vector2f newPos)
	{
		body.setPosition(newPos);
		t.setPosition(newPos);
	};
	void move(Vector2f normalizedMovementVec, Time cycleTime)
	{
		body.move({ normalizedMovementVec.x * 50 * cycleTime.asSeconds(), normalizedMovementVec.y * 50 * cycleTime.asSeconds() });
		t.move({ normalizedMovementVec.x * 50 * cycleTime.asSeconds(), normalizedMovementVec.y * 50 * cycleTime.asSeconds() });
	};
	void draw(RenderWindow& window)
	{
		//window.draw(netGhost);
		window.draw(body);
		window.draw(t);
	};

	bool isPossesed() { return possesed; };
	Vector2f getPos() { return body.getPosition(); };
};

vector<Player> playersVec;

Clock cycleTimer;
Time cycleTime;

IpAddress S_Ip;
unsigned short S_port;
string clientName;

NetworkClient netC;

void getUserInputData();

int main()
{
	RenderWindow window(sf::VideoMode(400, 400), "SFML works!");

	Texture t_player;
	t_player.loadFromFile("tank.png");
	Font font;
	font.loadFromFile("8bitOperatorPlus-Regular.ttf");

	getUserInputData();

	Player player(true);
	player.load(t_player, font, clientName);


	netC.init();

	netC.registerOnServer(S_Ip, S_port, clientName);

	vector<string> namesVec;
	netC.receiveConnectedClientsNames(namesVec);

	Packet receivedDataPacket;
	
	while (window.isOpen())
	{
		cycleTime = cycleTimer.restart();


		if (netC.receiveData(receivedDataPacket, S_Ip, S_port) == Socket::Status::Done)
		{
			if (receivedDataPacket.getDataSize() > 0)
			{
				string s;
				if (receivedDataPacket >> s)
				{
					if (s == "NEW")
					{
						if (receivedDataPacket >> s)
						{
							if (s != clientName)
							{
								namesVec.push_back(s);
								cout << "New player connected: " << namesVec.back() << endl;
							}
						}
					}
				}
			}
		}



		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == Event::KeyPressed) {}
		}
		if (window.hasFocus())
		{
			if (Keyboard::isKeyPressed(Keyboard::W))
				player.move({ 0, -1 }, cycleTime);
			if (Keyboard::isKeyPressed(Keyboard::A))
				player.move({ -1, 0 }, cycleTime);
			if (Keyboard::isKeyPressed(Keyboard::S))
				player.move({ 0, 1 }, cycleTime);
			if (Keyboard::isKeyPressed(Keyboard::D))
				player.move({ 1, 0 }, cycleTime);
		}

		window.clear();
		
		for (int i = 0; i < playersVec.size(); i++)
		{
			playersVec[i].draw(window);
		}

		player.draw(window);

		window.display();
	}

	return 0;
};

void getUserInputData()
{
	//cout << "Enter server IP: ";
	//cin >> serverIp;
	S_Ip = "localhost";
	cout << endl;
	cout << "Enter server registration port: ";
	cin >> S_port;
	cout << endl;
	cout << "Enter name: ";
	cin >> clientName;
};
