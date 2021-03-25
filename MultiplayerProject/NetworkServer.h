#pragma once
#pragma once
#include <SFML/Network.hpp>
#include <vector>
#include <iostream>

using namespace std;
using namespace sf;

struct Client
{
	string name;
	IpAddress Ip;
	UdpSocket* dataSocket;
	unsigned short port;
	Packet rDataPacket;
	Packet sDataPacket;
	bool done = true;

	Vector2f pos{ 5,5 };
};

class NetworkServer
{

	short regStep = 0;

	TcpListener listener;
	TcpSocket regSocket;

	Packet packet;

	Clock sendingsRateTimer;
	Int32 sendingsRate = 4;

public:
	vector<Client> clientsVec;

	NetworkServer();

	/// <summary>
	/// Call this once after creating a NetworkServer object. 
	/// </summary>
	/// <returns>status code</returns>
	Socket::Status init();

	/// <summary>
	/// Must be called in cycle. Executes in order:
	/// acceptIncomingConnection();
	/// receiveClientRegData();
	/// sendNewClientDataToAll();
	/// sendDedicatedDataPort();
	/// See their comments for details
	/// </summary>
	/// <returns>status code</returns>
	Socket::Status registerNewClients();


	/// <summary>
	/// Sends to just connected client names of every client that already connected
	/// In cycle should be executed after registerNewClients().
	/// Method is NON-blocking.
	/// </summary>
	/// <returns>status code</returns>
	Socket::Status sendConnectedClientsRecords();

	/// <summary>
	/// As soon as any of clients sent data to server this method will return Status::Done and 
	/// passed pointer will point to sender record. Use this method like this:
	/// if (receiveData(clientReceivedFrom) == Socket::Status::Done)
	/// {
	///		Extract received data using clientReceivedFrom->rDataPacket >> yourVar;
	/// }
	/// Method is NON-blocking. Use in cycle.
	/// </summary>
	/// <param name="clientReceivedFrom">Pointer for received client record</param>
	/// <returns>status code</returns>
	Socket::Status receiveData(unsigned int& receivedClientIndex);

	/// <summary>
	/// Use this to broadcast players positions and other data to clients.
	/// Fill packet with "DATA" and then clientName1  clientsData1  ...  clientName2  clientsData2 
	/// Method is NON-blocking.
	/// </summary>
	/// <param name="dataPacket">data to be sended to everyone</param>
	/// <returns>status code</returns>
	Socket::Status sendDataToAll(Packet dataPacket);

private:

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
	/// When new client connects - sends his name to all other connected clients so that they can create record for him
	/// </summary>
	/// <returns>status code</returns>
	Socket::Status sendNewClientDataToAll();

	/// <summary>
	/// Sends to client port of a udp socket that dedicated for connecting with this client.
	/// In cycle should be executed after receiveClientRegData().
	/// Method is NON-blocking.
	/// </summary>
	/// <returns>status code</returns>
	Socket::Status sendDedicatedDataPort();

};

