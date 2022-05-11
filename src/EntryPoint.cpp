#include "enet/enet.h"
#include <iostream>
#include <string>

struct NewPlayerPacket
{
	char playerName[32];
	char message[128];
};

void Host()
{
	std::cout << "Creating a server..." << std::endl;
	ENetAddress hostAddress;
	hostAddress.host = ENET_HOST_ANY;
	hostAddress.port = 1234;

	ENetHost* serverHost = enet_host_create(&hostAddress, 32, 2, 0, 0);

	if (serverHost == NULL)
	{
		std::cout << "An error occured while trying to create the host." << std::endl;
		return;
	}

	while(true)
	{
		ENetEvent receivedEvent;

		while (enet_host_service(serverHost, &receivedEvent, 100) > 0)
		{
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
					ENetPacket* receivedPacket = receivedEvent.packet;
					NewPlayerPacket* info = (NewPlayerPacket*)receivedPacket->data;
					std::cout << "A new player, " << info->playerName << ", has joined. They say: \"" << info->message << "\"" << std::endl;
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

	std::cout << "What do you want to say? (127 character limit)" << std::endl;
	std::string sentMessage;
	std::cin >> sentMessage;

	std::cout << "Which IP do you want to connect to?" << std::endl;
	std::string ipAddress;
	std::cin >> ipAddress;
	std::cout << "Attempting to connect to IP: " << ipAddress << " ..." << std::endl;

	ENetHost* clientHost = enet_host_create(nullptr, 1, 2, 0, 0);

	if (clientHost == NULL)
	{
		std::cout << "An error occured while trying to create the client." << std::endl;
		return;
	}

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
			switch(receivedEvent.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
					std::cout << "Connected to server!" << std::endl;
					serverPeer = receivedEvent.peer;
					{
						NewPlayerPacket playerInfo;
						strncpy_s(playerInfo.playerName, name.c_str(), min(32, name.size()));
						strncpy_s(playerInfo.message, sentMessage.c_str(), min(128, sentMessage.size()));

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