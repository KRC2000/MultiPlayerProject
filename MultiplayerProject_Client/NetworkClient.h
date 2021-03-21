#pragma once
#include <SFML/Network.hpp>
#include <vector>
#include <iostream>

using namespace std;
using namespace sf;

/*Object for managing connection to the server that handles connection using NetworkServer*/
class NetworkClient
{
	UdpSocket dataSocket;
	TcpSocket regSocket;

	IpAddress S_Ip;
	unsigned short S_regPort;
	unsigned short S_dataPort;
public:
	NetworkClient();

	/*Tries to bind udp data socket to passed port, if failed - endlessly tries to bind any other port
	Return Status::Done if binded to any port*/
	Socket::Status init(unsigned short preferablePort);

	Socket::Status registerOnServer(IpAddress serverIp, unsigned short serverRegPort, string clientName);

private:
	/*Connects registration tcp socket to server registration tcp socket*/
	Socket::Status connectRegTcpSocket(IpAddress serverIp, unsigned short serverRegPort);

	/*Sets registration tcp socket to be blocking, then sends using it data udp socket port and network client name to 
	registration socket of a server, so that server create new dedicated client record*/
	Socket::Status sendClientRecipientData(string clientName);

	/*NetworkServer creates new udp socket with its own separate port for each client to communicate with that client after registration,
	receive port dedicated to this client using this method*/
	Socket::Status recieveDedicatedDataServerPort();

};

