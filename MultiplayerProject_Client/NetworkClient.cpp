#include "NetworkClient.h"

NetworkClient::NetworkClient()
{
}

Socket::Status NetworkClient::init(unsigned short preferablePort)
{
	Socket::Status status = dataSocket.bind(preferablePort);

	if (status == Socket::Status::Done)
		return Socket::Status::Done;
	else
	{
		cout << "(!)init(): Failed to bind passed preferred port\n";
		do
		{
			unsigned short newPort = Socket::AnyPort;
			cout << "init(): Trying to bind other port - " << newPort << endl;
			status = dataSocket.bind(newPort);
			if (status != Socket::Status::Done)
				cout << "(!)init(): Failed to bind other port. Retrying...\n";

		} while (status != Socket::Done);

		cout << "init(): Successfully binded to other port - " << dataSocket.getLocalPort() << endl;
		return Socket::Status::Done;
	}
}

Socket::Status NetworkClient::registerOnServer(IpAddress serverIp, unsigned short serverRegPort, string clientName)
{
	if (connectRegTcpSocket(serverIp, serverRegPort) != Socket::Status::Done)
		return Socket::Status::Error;

	if (sendClientRecipientData(clientName) != Socket::Status::Done)
		return Socket::Status::Error;
	
}

Socket::Status NetworkClient::connectRegTcpSocket(IpAddress serverIp, unsigned short serverRegPort)
{
	if (regSocket.connect(serverIp, serverRegPort) == Socket::Status::Done)
	{
		cout << "registerOnServer()->connectRegTcpSocket(): Connected to server\n";
		return Socket::Status::Done;
	}
	else
	{
		cout << "(!)registerOnServer()->connectRegTcpSocket(): Error connecting to server!\n";
		return Socket::Status::Error;
	}
}

Socket::Status NetworkClient::sendClientRecipientData(string clientName)
{
	if (!regSocket.isBlocking()) regSocket.setBlocking(true);

	Packet tempPacket;
	tempPacket << dataSocket.getLocalPort() << clientName;

	if (regSocket.send(tempPacket) == Socket::Status::Done)
	{
		cout << "registerOnServer()->sendClientRecipientData(): Successfully sent client recipient data\n";
		return Socket::Status::Done;
	}
	else
	{
		cout << "(!)registerOnServer()->sendClientRecipientData(): Failed to send client recipient data\n";
		return Socket::Status::Error;
	}
}
