#include "NetworkServer.h"

NetworkServer::NetworkServer()
{
}

Socket::Status NetworkServer::acceptIncomingConnection()
{
	if (regStep == 0)
	{
		if (listener.isBlocking()) listener.setBlocking(false);

		if (listener.accept(regSocket) == Socket::Status::Done)
		{
			cout << "acceptIncomingConnection(): Accepted new connection\n";
			regStep++;
			return Socket::Status::Done;
		}
		else return Socket::Status::NotReady;
	}
}

Socket::Status NetworkServer::receiveClientRegData()
{
	if (regStep == 1)
	{
		if (listener.isBlocking()) listener.setBlocking(false);

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
			regStep++;
			return Socket::Status::Done;
		}
		else return Socket::Status::NotReady;
	}
}
