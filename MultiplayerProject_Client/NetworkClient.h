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
};

