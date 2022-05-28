#include "SNET_Server.h"
#include <iostream>

void StrafeNetServer::AddPlayer(const int id)
{
	PlayerPacket newData;
	newData.playerID = id;
	playerDataMap.emplace(id, newData);
}

void StrafeNetServer::RemovePlayer(const int id)
{
	playerDataMap.erase(id);
}

const int StrafeNetServer::GetPlayerCount() const
{
	return playerDataMap.size();
}

void StrafeNetServer::SendData()
{
	//
}

void StrafeNetServer::ReceiveData()
{
	ENetEvent receivedEvent;
	unsigned int timeout = 30;
	while (enet_host_service(host, &receivedEvent, timeout) > 0)
	{
		switch (receivedEvent.type)
		{
			case ENET_EVENT_TYPE_CONNECT:
			{
				std::cout << "New client connected to server." << std::endl;

			}
			break;

			case ENET_EVENT_TYPE_DISCONNECT:
			{
				std::cout << "Client disconnected from server." << std::endl;
			}
		}
	}
}


int StrafeNetServer::Initialise()
{
	if (enet_initialize() != 0)
		return EXIT_FAILURE;

	std::cout << "ENet initialised." << std::endl;

	ENetAddress hostAddress;
	hostAddress.host = ENET_HOST_ANY;
	hostAddress.port = 1234;
	
	int numConnections = 32;
	int numChannels = 2;
	host = enet_host_create(&hostAddress, numConnections, numChannels, 0, 0);

	if (host == NULL)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
