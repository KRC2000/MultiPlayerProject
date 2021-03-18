#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <vector>

using namespace sf;
using namespace std;

struct Client
{
	string name;
	IpAddress ip;
	Uint16 port;
	UdpSocket* serverDataSocket;
	Vector2f pos{0,0};
};

vector<Client> clientsVec;

	Clock dataBroadcastTimer;
	IpAddress clientIp = IpAddress::LocalHost;

	//UdpSocket dataSocket;
	TcpSocket regSocket;
	TcpListener listener;
	SocketSelector selector;

	Packet packet;


void bindSockets();

int main()
{
	bindSockets();
	
	
	selector.add(listener);
	

	packet.clear();

	

	while (true)
	{
		if (selector.wait(milliseconds(30)))
		{
			for (int i = 0; i < clientsVec.size(); i++)
			{
				if (selector.isReady(*clientsVec[i].serverDataSocket))
				{
					packet.clear();
					if (clientsVec[i].serverDataSocket->receive(packet, clientsVec[i].ip, clientsVec[i].port) == Socket::Status::Done)
					{
						float data;
					
						while (!packet.endOfPacket())
						{
							if (packet >> data)
							{
								clientsVec[i].pos.x = data;
							}
							if (packet >> data)
							{
								clientsVec[i].pos.y = data;
							}
						}
					}
					packet.clear();
				}
			}
			if (selector.isReady(listener))
			{
				if (listener.accept(regSocket) == sf::Socket::Done)
				{
					cout << ">>Listener accepted connection to registration socket from: " << regSocket.getRemoteAddress() << ":" << regSocket.getRemotePort() << endl;
					if (regSocket.receive(packet) == Socket::Status::Done)
					{
						cout << ">>Registration socket received packet with client data port\n";
						string clientName;
						Uint16 clientDataPort;
						packet >> clientDataPort >> clientName;
						cout << ">>Client data port is: " << clientDataPort << endl;
						Client client;
						clientsVec.push_back(client);
						clientsVec.back().ip = regSocket.getRemoteAddress();
						clientsVec.back().port = clientDataPort;
						clientsVec.back().name = clientName;
						clientsVec.back().serverDataSocket = new UdpSocket;
						clientsVec.back().serverDataSocket->bind(Socket::AnyPort);
						selector.add(*clientsVec.back().serverDataSocket);
						if (clientsVec.back().serverDataSocket->bind(Socket::AnyPort) == sf::Socket::Done)
							cout << ">>New server-client data socket binded successfully to port: " << clientsVec.back().serverDataSocket->getLocalPort() << endl;
						else cout << "!!!server-client data socket bind error!!!\n";

						cout << ">>Client record created: " << clientsVec.back().ip << ":" << clientsVec.back().port << " " << clientsVec.back().name << endl;
						packet.clear();

						packet << (Uint16)clientsVec.back().serverDataSocket->getLocalPort();
						if (regSocket.send(packet) == Socket::Status::Done)
							cout << ">>Sent data socket port to client\n";
						else cout << "!!!Failed to send data socket port to client!!!\n";
						packet.clear();

						if (clientsVec.size() > 1)
						{
							for (unsigned int i = 0; i < clientsVec.size() - 1; i++)
							{
								packet << clientsVec[i].name;
							}

							if (regSocket.send(packet) == Socket::Status::Done)
								cout << ">>Sent client records to the new client\n";
							else cout << "!!!Failed to send client records to the new client!!!\n";
							packet.clear();

							packet << "NEW" << clientsVec.back().name;
							for (unsigned int i = 0; i < clientsVec.size() - 1; i++)
							{
								if (clientsVec[i].serverDataSocket->send(packet, clientsVec[i].ip, clientsVec[i].port) == Socket::Status::Done)
									cout << ">>Sent new client record to client - " << clientsVec[i].name << endl;
								else cout << "!!!Failed to send client record to every client!!!\n";
							}
						}
						else
						{
							packet << "FIRST";
							if (regSocket.send(packet) == Socket::Status::Done)
								cout << ">>Sent message about being first client\n";
							else cout << "!!!Failed to send message about being first client!!!\n";
							packet.clear();
						}

					}
					else cout << "!!!Failed to receive packet with client data port!!!\n";
				}
				else cout << "!!!Listener NOT accepted connection, ERROR!!!\n";

			}
		}


			packet.clear();
			for (int i = 0; i < clientsVec.size(); i++)
			{
				packet << "DATA";
				for (int n = 0; n < clientsVec.size(); n++)
				{
					if (n != i)
						packet << clientsVec[n].name << clientsVec[n].pos.x << clientsVec[n].pos.y;
				}
				clientsVec[i].serverDataSocket->send(packet, clientsVec[i].ip, clientsVec[i].port);
				packet.clear();
			}
			dataBroadcastTimer.restart();
			packet.clear();
	}


	getchar();
	return 0;
}

void bindSockets()
{
	/*if (dataSocket.bind(Socket::AnyPort) == sf::Socket::Done)
		cout << ">>Data socket binded successfully to port: " << dataSocket.getLocalPort() << endl;
	else cout << "!!!Data socket bind error!!!\n";*/

	if (listener.listen(Socket::AnyPort) == Socket::Done)
		cout << ">>Listener binded successfully to port: " << listener.getLocalPort() << endl;
	else cout << "!!!Listener socket bind error!!!\n"; 
}