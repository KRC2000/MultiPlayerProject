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

	if  (recieveDedicatedDataServerPort() != Socket::Status::Done)
		return Socket::Status::Error;
	
}

Socket::Status NetworkClient::receiveConnectedClientsNames(vector<string>& namesVec)
{
	if (!regSocket.isBlocking()) regSocket.setBlocking(true);

	Packet tempPacket;

	if (regSocket.receive(tempPacket) == Socket::Status::Done)
	{
		if (tempPacket.getDataSize() > 0)
		{
			while (!tempPacket.endOfPacket())
			{
				string name;
				if (tempPacket >> name)
				{
					if (name == "FIRST")
					{
						"receiveConnectedClientsNames(): No clients are connected, you are first\n";
						return Socket::Status::Done;
					}
					namesVec.push_back(name);
				}
				else
				{
					cout << "(!)receiveConnectedClientsNames() : Failed to read packet\n";
					return Socket::Status::Error;
				}
			}
			cout << "receiveConnectedClientsNames() :Client names read\n";
			return Socket::Status::Done;

		}
		else cout << "(!)receiveConnectedClientsNames(): Receives packet is empty, ensure that packet contains: (string name1 << string name2 << ...) or \"FIRST\" if it's first connected client\n";
	}
	else cout << "(!)receiveConnectedClientsNames(): Failed to receive clients names\n";

	return Socket::Status::Error;
}

Socket::Status NetworkClient::receiveData(Packet& dataPacket, IpAddress S_Ip, unsigned short S_dataPort)
{
	if (dataSocket.isBlocking())dataSocket.setBlocking(false);

	if (dataSocket.receive(dataPacket, S_Ip, S_dataPort) == Socket::Status::Done)
	{
		if (dataPacket.getDataSize() > 0)
		{
			cout << "receiveData(): Data received\n";
			return Socket::Status::Done;
		}
		else
		{
			cout << "(!)receiveData(): Received packet is empty\n";
			return Socket::Status::Error;
		}
	}
	return Socket::Status::NotReady;
}

Socket::Status NetworkClient::sendData(Packet& dataPacket, IpAddress S_Ip, unsigned short S_dataPort)
{
	if (dataSocket.isBlocking())dataSocket.setBlocking(false);

	if (dataPacket.getDataSize() < 1)
	{
		cout << "(!)sendData(): Error, packet is empty\n";
		return Socket::Status::Error;
	}

	if (dataSocket.send(dataPacket, S_Ip, S_dataPort) == Socket::Status::Done)
	{
		cout << "sendData(): Data was sent\n";
		return Socket::Status::Done;
	}
	else return Socket::Status::NotReady;
}

Socket::Status NetworkClient::connectRegTcpSocket(IpAddress serverIp, unsigned short serverRegPort)
{
	if (!regSocket.isBlocking()) regSocket.setBlocking(true);

	if (regSocket.connect(serverIp, serverRegPort) == Socket::Status::Done)
	{
		cout << "connectRegTcpSocket(): Connected to server\n";
		S_Ip = serverIp;
		S_dataPort = serverRegPort;
		return Socket::Status::Done;
	}
	else
	{
		cout << "(!)connectRegTcpSocket(): Error connecting to server!\n";
		return Socket::Status::Error;
	}
}

Socket::Status NetworkClient::sendClientRecipientData(string clientName)
{
	if (!regSocket.isBlocking()) regSocket.setBlocking(true);

	Packet tempPacket;
	tempPacket << clientName << static_cast<Uint16>(dataSocket.getLocalPort());

	if (regSocket.send(tempPacket) == Socket::Status::Done)
	{
		cout << "sendClientRecipientData(): Successfully sent client recipient data\n";
		return Socket::Status::Done;
	}
	else
	{
		cout << "(!)sendClientRecipientData(): Failed to send client recipient data\n";
		return Socket::Status::Error;
	}
}

Socket::Status NetworkClient::recieveDedicatedDataServerPort()
{
	if (!regSocket.isBlocking()) regSocket.setBlocking(true);

	Packet tempPacket;

	if (regSocket.receive(tempPacket) == Socket::Status::Done)
	{
		if (tempPacket.getDataSize() > 0)
		{
			if (tempPacket.getDataSize() == sizeof(Uint16))
			{
				if (tempPacket >> S_dataPort)
				{
					cout << "recieveDedicatedDataServerPort(): Successfully received data client-dedicated port of a server - " << S_dataPort << endl;
					return Socket::Status::Done;
				}
				else cout << "(!)recieveDedicatedDataServerPort(): Failed to read from received packet\n";
			}
			else cout << "(!)recieveDedicatedDataServerPort(): Invalid packet size, ensure that server sends only Uint16 var\n";
		}
		else cout << "(!)recieveDedicatedDataServerPort(): Received packet is empty\n";
	}
	else cout << "(!)recieveDedicatedDataServerPort(): Failed to receive client-dedicated port of a server\n";

	return Socket::Status::Error;
}
