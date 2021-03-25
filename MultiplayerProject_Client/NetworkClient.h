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

	Packet sendPacket;

	Clock sendRateTimer;
	Int16 sendRate = 2;

	IpAddress S_Ip;
	unsigned short S_regPort;
	unsigned short S_dataPort;
public:
	NetworkClient();

	/////// Execute these ones ///////

	/*Tries to bind udp data socket to passed port, if failed - endlessly tries to bind any other port
	Return Status::Done if binded to any port*/
	Socket::Status init(unsigned short preferablePort = Socket::AnyPort);

	/*Setups connection to the server, retrieves and sends needed data to exchange data*/
	Socket::Status registerOnServer(IpAddress serverIp, unsigned short serverRegPort, string clientName);

	/*Receives and records connected clients names to passed vector*/
	Socket::Status receiveConnectedClientsNames(vector<string>& namesVec);

	/////// Execute these in main loop ///////

	/// <summary>
	/// Receives data sent by server's udp socket dedicated for connection with this client.
	/// Receive is NON-blocking.
	/// When Status::Done returned, you can extract dataPacket.
	/// Packet dataPacket must exist as long ass this method being called
	/// </summary>
	/// <param name="dataPacket"> packet to fill with received data</param>
	/// <param name="S_Ip"> Server Ip address</param>
	/// <param name="S_dataPort"> Server data port for connection with this client</param>
	/// <returns>status code</returns>
	Socket::Status receiveData(Packet& dataPacket, IpAddress S_Ip, unsigned short S_dataPort);


	/// <summary>
	/// Sends copy of passed packet to server's udp socket dedicated for connection with this client.
	/// Sending is NON-blocking.
	/// Use this method to send player location, rotation, events - by using event codes - numbers or strings
	/// To change default frequency see setSendFreq();
	/// If Status::Done returned, packet was sent.
	/// </summary>
	/// <param name="dataPacket">Packet to sent</param>
	/// <returns>status code</returns>
	Socket::Status sendData(Packet dataPacket);


	/// <summary>
	/// Set sentData() send frequency 
	/// </summary>
	/// <param name="milliseconds">idle time between send calls in milliseconds</param>
	void setSendFreq(Int32 milliseconds);
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

