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
		Packet dataPacket;
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
	/// Needs to be executed first.
	/// Method is NON-blocking.
	/// </summary>
	/// <returns>status code</returns>
	Socket::Status acceptIncomingConnection();

	/// <summary>
	/// Receives clients network name and data port, creates Client record in clientsVec and fills it up with received information.
	/// Will be executed only if acceptIncomingConnection() was previously called.
	/// In cycle should be executed after acceptIncomingConnection().
	/// Method is NON-blocking.
	/// </summary>
	/// <returns>status code</returns>
	Socket::Status receiveClientRegData();

	/// <summary>
	/// Sends to client port of a udp socket that dedicated for connecting with this client.
	/// In cycle should be executed after receiveClientRegData().
	/// Method is NON-blocking.
	/// </summary>
	/// <returns>status code</returns>
	Socket::Status sendDedicatedDataPort();

	/// <summary>
	/// Sends to just connected client names of every client that already connected
	/// In cycle should be executed after sendDedicatedDataPort().
	/// Method is NON-blocking.
	/// </summary>
	/// <returns>status code</returns>
	Socket::Status sendConnectedClientsRecords();

	/// <summary>
	/// As soon as any of clients sent data to server this method will return Status::Done and 
	/// passed pointer will point to sender record. Use this method like this:
	/// if (receiveData(clientReceivedFrom) == Socket::Status::Done)
	/// {
	///		Extract received data using clientReceivedFrom->dataPacket >> yourVar;
	/// }
	/// </summary>
	/// <param name="clientReceivedFrom">Pointer for received client record</param>
	/// <returns>status code</returns>
	Socket::Status receiveData(Client* clientReceivedFrom);
};

