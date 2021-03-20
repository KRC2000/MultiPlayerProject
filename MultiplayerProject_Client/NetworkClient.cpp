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
		cout << "init(): Failed to bind passed preferred port\n";
		do
		{
			unsigned short newPort = Socket::AnyPort;
			cout << "init(): Trying to bind other port - " << newPort << endl;
			status = dataSocket.bind(newPort);
			if (status != Socket::Status::Done)
				cout << "init(): Failed to bind other port. Retrying...\n";

		} while (status != Socket::Done);

		cout << "init(): Successfully binded to other port - " << dataSocket.getLocalPort() << endl;
	}
}
