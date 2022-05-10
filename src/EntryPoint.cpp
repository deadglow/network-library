#include "enet/enet.h"
#include <iostream>
#include <string>

#define IS_HOST false

void Host()
{
	std::cout << "Creating a server..." << std::endl;
	ENetAddress hostAddress;
	hostAddress.host = ENET_HOST_ANY;
	hostAddress.port = 1234;

	ENetHost* serverHost = enet_host_create(&hostAddress, 32, 2, 0, 0);

	while(true)
	{
		ENetEvent receivedEvent;

		while (enet_host_service(serverHost, &receivedEvent, 100) > 0)
		{
			std::cout << "Received a packet!" << std::endl;
			switch (receivedEvent.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
					std::cout << "A new client connected to the server." << std::endl;
				break;
				
				case ENET_EVENT_TYPE_DISCONNECT:
					std::cout << "A client disconnected from the server." << std::endl;
				break;
				
				case ENET_EVENT_TYPE_RECEIVE:
					std::cout << "Received user defined packet." << std::endl;
				break;
				
				default:
					std::cout << "UNKNOWN." << std::endl;
				break;
			}
		}
	}
}

void Client()
{
	std::cout << "Creating a client..." << std::endl;

	std::cout << "Which IP do you want to connect to?" << std::endl;
	std::string ipAddress;
	std::cin >> ipAddress;
	std::cout << "Attempting to connect to IP: " << ipAddress << " ..." << std::endl;

	ENetHost* clientHost = enet_host_create(nullptr, 1, 2, 0, 0);

	ENetAddress address;
	address.port = 1234;
	enet_address_set_host(&address, ipAddress.c_str());

	enet_host_connect(clientHost, &address, 2, 5000);

	while (true)
	{
		ENetEvent receivedEvent;

		while (enet_host_service(clientHost, &receivedEvent, 100) > 0)
		{
			std::cout << "Client received a packet." << std::endl;
			switch(receivedEvent.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
					std::cout << "Connected to server!" << std::endl;
				break;
				
				case ENET_EVENT_TYPE_DISCONNECT:
					std::cout << "Disconnected from server." << std::endl;
				break;
				
				case ENET_EVENT_TYPE_RECEIVE:
					std::cout << "Received user defined packet." << std::endl;
				break;
				
				default:
					std::cout << "UNKNOWN." << std::endl;
				break;
			}
		}
	}
}

int main()
{
    if (enet_initialize() != 0)
    {
        std::cout << "An error occured while initializing ENet." << std::endl;
        return EXIT_FAILURE;
    }
    else
        std::cout << "ENet initialized." << std::endl;

	char input;

	std::cout << "Is this instance a host?" << std::endl;

	std::cin >> input;

	bool isHost = false;

	if (input == 'y')
		isHost = true;


    if (isHost)
    {
        Host();
    }
    else
    {
        Client();
    }

    enet_deinitialize();
    return 0;
}