#pragma once
#pragma once
#include <SFML/Network.hpp>
#include <vector>
#include <iostream>

using namespace std;
using namespace sf;

class NetworkServer
{
	struct Client
	{
		string name;
		IpAddress Ip;
		UdpSocket *dataSocket;
		unsigned short port;
	};
	vector<Client> clientsVec;

	short regStep = 0;

	TcpListener listener;
	TcpSocket regSocket;

	Packet packet;

public:
	NetworkServer();

	/// <summary>
	/// Listener accepts incoming tcp registration connection.
	/// Method is NON-blocking
	/// </summary>
	/// <returns>status code</returns>
	Socket::Status acceptIncomingConnection();

	/// <summary>
	/// Receives clients network name and data port, creates Client record in clientsVec and fills it up with received information.
	/// Will be executed only if acceptIncomingConnection() was previously called.
	/// </summary>
	/// <returns>status code</returns>
	Socket::Status receiveClientRegData();
};

