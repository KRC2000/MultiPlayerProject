#include "NetworkServer.h"

NetworkServer::NetworkServer()
{
}

Socket::Status NetworkServer::registerNewClients()
{
	acceptIncomingConnection();
	receiveClientRegData();
	sendDedicatedDataPort();
}

Socket::Status NetworkServer::acceptIncomingConnection()
{
	if (regStep == 0)
	{
		if (listener.isBlocking()) listener.setBlocking(false);

		if (listener.accept(regSocket) == Socket::Status::Done)
		{
			cout << "acceptIncomingConnection(): Accepted new connection\n";
			regStep = 1;
			return Socket::Status::Done;
		}
		else return Socket::Status::NotReady;
	}
}

Socket::Status NetworkServer::receiveClientRegData()
{
	if (regStep == 1)
	{
		if (regSocket.isBlocking()) regSocket.setBlocking(false);

		if (regSocket.receive(packet) == Socket::Status::Done)
		{
			if (packet.getDataSize() > 0)
			{
				string name;
				if (packet >> name)
				{
					Client newClient;
					clientsVec.push_back(newClient);
					clientsVec.back().name = name;
					clientsVec.back().Ip = regSocket.getRemoteAddress();
					clientsVec.back().dataSocket = new UdpSocket;
					if (clientsVec.back().dataSocket->bind(Socket::AnyPort) != Socket::Status::Done)
						cout << "(!)receiveClientRegData(): Failed to bind port to the new client-dedicated data port\n";
				}
				else
				{
					cout << "(!)receiveClientRegData(): Failed to read client name from received packet\n";
					return Socket::Status::Error;
				}

				Uint16 port;
				if (packet >> port)
				{
					clientsVec.back().port = static_cast<unsigned short>(port);
				}
				else
				{
					cout << "(!)receiveClientRegData(): Failed to read client data socket port from received packet\n";
					return Socket::Status::Error;
				}

				if (!packet.endOfPacket())
					cout << "(!)receiveClientRegData(): Client registration data received, but something left, data probably corrupted\n";
			}
			else
			{
				cout << "(!)receiveClientRegData(): Error, received packet is empty\n";
				return Socket::Status::Error;
			}

			cout << "receiveClientRegData(): Client registration data received\n";
			regStep = 2;
			packet.clear();
			return Socket::Status::Done;
		}
		else return Socket::Status::NotReady;
	}
}

Socket::Status NetworkServer::sendDedicatedDataPort()
{
	if (regStep == 2)
	{
		if (regSocket.isBlocking()) regSocket.setBlocking(false);

		if (packet.getDataSize() == 0)
			packet << static_cast<Uint16>(clientsVec.back().dataSocket->getLocalPort());

		if (regSocket.send(packet) == Socket::Status::Done)
		{
			cout << "sendDedicatedDataPort(): Dedicated data port sent\n";
			regStep = 3;
			return Socket::Status::Done;
		}
		else return Socket::Status::NotReady;
	}
}

Socket::Status NetworkServer::sendConnectedClientsRecords()
{
	if (regStep = 3)
	{
		if (regSocket.isBlocking()) regSocket.setBlocking(false);

		if (packet.getDataSize() == 0)
		{
			if (clientsVec.size() > 1)
			{
				for (int i = 0; i < clientsVec.size() - 1; i++)
					packet << clientsVec[i].name;
			}
			else packet << "FIRST";
		}

		if (regSocket.send(packet) == Socket::Status::Done)
		{
			cout << "sendConnectedClientsRecords(): Connected clients records sent to new client\n";
			regStep = 0;
			regSocket.disconnect();
			return Socket::Status::Done;
		}
		else return Socket::Status::NotReady;
	}
}

Socket::Status NetworkServer::receiveData(Client* clientReceivedFrom)
{
	for (int i = 0; i < clientsVec.size(); i++)
	{
		if (clientsVec[i].dataSocket->isBlocking()) clientsVec[i].dataSocket->setBlocking(false);
		IpAddress tempIp = clientsVec[i].Ip;
		unsigned short tempPort = clientsVec[i].port;

		if (clientsVec[i].dataSocket->receive(clientsVec[i].rDataPacket, tempIp, tempPort) == Socket::Status::Done)
		{
			clientReceivedFrom = &clientsVec[i];
			return Socket::Status::Done;
		}
	}

	return Socket::Status::NotReady;
}

Socket::Status NetworkServer::sendDataToAll(Packet* dataPacket)
{
	for (int i = 0; i < clientsVec.size(); i++)
	{
		if (clientsVec[i].dataSocket->isBlocking()) clientsVec[i].dataSocket->setBlocking(false);
		IpAddress tempIp = clientsVec[i].Ip;
		unsigned short tempPort = clientsVec[i].port;

		if (clientsVec[i].dataSocket->send(*dataPacket, tempIp, tempPort) == Socket::Status::Done)
			return Socket::Status::Done;
	}
	return Socket::Status::NotReady;
}
