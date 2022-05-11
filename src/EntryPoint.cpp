#include "enet/enet.h"
#include <iostream>
#include <string>

struct NewPlayerPacket
{
	char playerName[32];
	float positionX;
	float positionY;
};

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
				{
					std::cout << "Received user defined packet." << std::endl;
					ENetPacket* receivedPacket = receivedEvent.packet;
					NewPlayerPacket* info = (NewPlayerPacket*)receivedPacket->data;
					std::cout << "A new player, " << info->playerName << ", has joined at (";
					std::cout << info->positionX << ", " << info->positionY << ")!" << std::endl;
				}
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

	std::cout << "What is your name?" << std::endl;
	std::string name;
	std::cin >> name;
	if (name.length() > 31)
		name[31] = '\0';

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
		ENetPeer *serverPeer = nullptr;

		while (enet_host_service(clientHost, &receivedEvent, 100) > 0)
		{
			std::cout << "Client received a packet." << std::endl;
			switch(receivedEvent.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
					std::cout << "Connected to server!" << std::endl;
					serverPeer = receivedEvent.peer;
					{
						NewPlayerPacket playerInfo;
						strncpy_s(playerInfo.playerName, name.c_str(), 32);
						playerInfo.positionX = 3.0f;
						playerInfo.positionY = 4.0f;

						ENetPacket *infoPacket = enet_packet_create(&playerInfo, sizeof(NewPlayerPacket), ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(serverPeer, 0, infoPacket);
					}
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