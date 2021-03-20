#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <vector>

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


Clock cycleTimer, dataSendTimer;
Time cycleTime;

UdpSocket dataSocket;
TcpSocket regSocket;
Packet sPacket, rPacket;
IpAddress serverIp;
string clientName;
Uint16 regServerPort;
Uint16 dataServerPort;

void getUserInputData();
void loadResources(Texture& t, Font& f);
void bindSockets();
void connectRegTcpSocket();
void sendDataUdpPort();
void recieveDataServerPort();


int main()
{
	RenderWindow window(sf::VideoMode(400, 400), "SFML works!");

	Texture t_player;
	Font font;

	getUserInputData();

	loadResources(t_player, font);

	Player player(true);
	player.load(t_player, font, clientName);


	bindSockets();

	connectRegTcpSocket();

	sendDataUdpPort();

	recieveDataServerPort();
	

	if (regSocket.receive(rPacket) == Socket::Status::Done)
	{
		string s;
		while (!rPacket.endOfPacket())
		{
			if (rPacket >> s)
			{
				if (s != "FIRST")
				{
					Player p;
					playersVec.push_back(p);
					playersVec.back().load(t_player, font, s);
					cout << ">>Created record: " << playersVec.back().name << endl;
				}
				else cout << ">>You are first connected client\n";
			}
		}
		rPacket.clear();
		cout << ">>Successfully received client records" << endl;
	}
	else cout << "!!!Failed to receive client records!!!\n";




	
	while (window.isOpen())
	{
		cycleTime = cycleTimer.restart();

		dataSocket.setBlocking(false);

		IpAddress tempIp = serverIp;
		Uint16 tempPort = dataServerPort;
		if (dataSocket.receive(rPacket, tempIp, tempPort) == Socket::Status::Done)
		{
			string s;
			if (!rPacket.endOfPacket())
			{
				if (rPacket >> s)
				{
					if (s == "NEW")
					{
						if (rPacket >> s)
						{
							Player p;
							playersVec.push_back(p);
							playersVec.back().load(t_player, font, s);
							cout << ">>New client connected to the server - " << playersVec.back().name << endl;
						}
						else cout << "!!!Reading data error!!!\n";
					}

					if (s == "DC")
					{
						if (rPacket >> s)
						{
							for (int i = 0; i < playersVec.size(); i++)
							{
								if (playersVec[i].name == s)
								{
									playersVec.erase(playersVec.begin() + i);
									break;
									cout << ">>" << s << " disconnected from the server.\n";
								}
							}
						}
					}

					if (s == "DATA")
					{
						while (!rPacket.endOfPacket())
						{
							if (rPacket >> s)
							{
								for (int i = 0; i < playersVec.size(); i++)
								{
									if (s == playersVec[i].name)
									{
										float x, y;
										rPacket >> x >> y;
										playersVec[i].setPosition({ x, y });
										//cout << "c";
									}
								}
							}
						}
					}
				}
				else cout << "!!!Reading data error!!!\n";
			}
			else rPacket.clear();
		}

		//dataSocket.setBlocking(true);
		
		if (dataSendTimer.getElapsedTime().asMilliseconds() > 8)
		{
			if (sPacket.getDataSize() == 0)
			{
				sPacket << player.getPos().x << player.getPos().y;
			}

			IpAddress tempIp = serverIp;
			Uint16 tempPort = dataServerPort;
			if (dataSocket.send(sPacket, tempIp, tempPort) == Socket::Status::Done)
			{
				//cout << "Done: " << dataSendTimer.getElapsedTime().asMilliseconds() << endl;
				//cout << "Size: " << sPacket.getDataSize() << endl << endl;
				dataSendTimer.restart();
				sPacket.clear();
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

void bindSockets()
{
	if (dataSocket.bind(Socket::AnyPort) == sf::Socket::Done)
		cout << ">>Data socket binded successfully to port: " << dataSocket.getLocalPort() << endl;
	else cout << "!!!Data socket bind error!!!\n";
};

void getUserInputData()
{
	//cout << "Enter server IP: ";
	//cin >> serverIp;
	serverIp = "localhost";
	cout << endl;
	cout << "Enter server registration port: ";
	cin >> regServerPort;
	cout << endl;
	cout << "Enter name: ";
	cin >> clientName;
};

void connectRegTcpSocket()
{
	if (regSocket.connect(serverIp, regServerPort) == Socket::Status::Done)
		cout << ">>Registration socket connected to server!\n";
	else cout << "!!!Registration socket connection error!!!\n";
};

void sendDataUdpPort()
{
	sPacket << dataSocket.getLocalPort() << clientName;
	if (regSocket.send(sPacket) == Socket::Status::Done)
		cout << ">>Successfully sent data udp socket port to server\n";
	else cout << "!!!Sending data udp socket port to server failed!!!\n";

	sPacket.clear();
};

void loadResources(Texture& t, Font& f)
{
	t.loadFromFile("tank.png");
	f.loadFromFile("8bitOperatorPlus-Regular.ttf");
};

void recieveDataServerPort()
{
	if (regSocket.receive(rPacket) == Socket::Status::Done)
	{
		rPacket >> dataServerPort;
		cout << ">>Successfully received data dedicated socket port of a server: " << dataServerPort << endl;
	}
	else cout << "!!!Failed to receive dedicated data socket port of a server!!!\n";

	rPacket.clear();
};