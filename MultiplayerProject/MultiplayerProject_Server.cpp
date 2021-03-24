#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <vector>

#include "NetworkServer.h"

using namespace sf;
using namespace std;

NetworkServer netS;


int main()
{
	
	netS.init();
	


	while (true)
	{
		netS.registerNewClients();
		netS.sendConnectedClientsRecords();
	}


	getchar();
	return 0;
}
