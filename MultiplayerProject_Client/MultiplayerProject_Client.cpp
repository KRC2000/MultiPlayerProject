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
	void load(Texture& texture, Font& font)
	{
		body.setTexture(texture);
		body.setTextureRect(IntRect(0, 0, texture.getSize().x / 4, texture.getSize().y / 4));
		body.setScale(2, 2);
		if (!possesed) body.setColor(Color::Red);
		netGhost.setTexture(texture);
		//name = playerName;

		t.setFont(font);
		t.setString(name);
		t.setFillColor(sf::Color::Red);
		t.setPosition(body.getGlobalBounds().width / 2 - t.getGlobalBounds().width / 2, body.getPosition().y - t.getGlobalBounds().height);
	};
	void setPosition(Vector2f newPos)
	{
		body.setPosition(newPos);
		t.setPosition(newPos.x + body.getGlobalBounds().width / 2 - t.getGlobalBounds().width / 2, body.getPosition().y - t.getGlobalBounds().height);
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

Player player(true);

void getUserInputData(string& playerName);
void addPlayer(Texture& t_player, Font& font, string clientName);

int main()
{
	RenderWindow window(sf::VideoMode(400, 400), "SFML works!");


	Texture t_player;
	t_player.loadFromFile("indianajones.png");
	Font font;
	font.loadFromFile("8bitOperatorPlus-Regular.ttf");

	
	getUserInputData(player.name);
	player.load(t_player, font);




	netC.init();
	netC.registerOnServer(S_Ip, S_port, player.name);

	vector<string> namesVec;
	netC.receiveConnectedClientsNames(namesVec);
	for (int i = 0; i < namesVec.size(); i++)
	{
		addPlayer(t_player, font, namesVec[i]);
	}

	Packet receivedDataPacket;
	Packet sendDataPacket;


	
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
								addPlayer(t_player, font, s);
								cout << "New player connected: " << playersVec.back().name << endl;
							}
						}
					}
					if (s == "DATA")
					{
						while (!receivedDataPacket.endOfPacket())
						{
							float x, y;
							receivedDataPacket >> s;
							receivedDataPacket >> x;
							receivedDataPacket >> y;
							for (int i = 0; i < playersVec.size(); i++)
							{
								if (s == playersVec[i].name)
									playersVec[i].setPosition({ x, y });
							}
						}
					}
				}
			}
		}

		sendDataPacket.clear();
		sendDataPacket << "DATA" << player.getPos().x << player.getPos().y;
		netC.sendData(sendDataPacket);




		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
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

void getUserInputData(string& playerName)
{
	//cout << "Enter server IP: ";
	//cin >> serverIp;
	S_Ip = "localhost";
	cout << endl;
	cout << "Enter server registration port: ";
	cin >> S_port;
	cout << endl;
	cout << "Enter name: ";
	cin >> playerName;
};

void addPlayer(Texture& t_player, Font& font, string clientName)
{
	Player p;
	playersVec.push_back(p);
	playersVec.back().name = clientName;
	playersVec.back().load(t_player, font);
};