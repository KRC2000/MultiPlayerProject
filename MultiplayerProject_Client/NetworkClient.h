#pragma once
#include <SFML/Network.hpp>
#include <vector>
#include <iostream>

using namespace std;
using namespace sf;

class NetworkClient
{
	UdpSocket dataSocket;
	TcpSocket regSocket;

	IpAddress S_Ip;
	Uint16 S_regPort;
	Uint16 S_dataPort;
public:
	NetworkClient();


	Socket::Status init(unsigned short preferablePort);
};

