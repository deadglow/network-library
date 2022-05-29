#include "SNET_Server.h"
#include <iostream>

SNET_Server::~SNET_Server()
{
	// remove all players
	while (connectedUsers.size() > 0)
		RemoveConnectedUser(connectedUsers.begin()->first);

	if (host != nullptr)
		enet_host_destroy(host);

	enet_deinitialize();
}

int SNET_Server::Initialise()
{
	if (enet_initialize() != 0)
	{
		std:: cout << "ENet failed to initialise.\n";
		return EXIT_FAILURE;
	}

	std::cout << "ENet successfully initialised.\n";

	ENetAddress hostAddress;
	hostAddress.host = ENET_HOST_ANY;
	hostAddress.port = SNET_DEFAULT_PORT;

	host = enet_host_create(&hostAddress, SNET_MAX_CONNECTIONS, SNET_CHANNEL_COUNT, 0, 0);

	if (host == nullptr)
	{
		std::cout << "Failed to create host.\n";
		return EXIT_FAILURE;
	}
	
	std::cout << "Created server on port " << hostAddress.port << "\n";

	return EXIT_SUCCESS;
}

void SNET_Server::Update()
{
	// Shuttle packets and receive data from clients
	ENetEvent receivedEvent;
	while (enet_host_service(host, &receivedEvent, 0) > 0)
	{
		switch (receivedEvent.type)
		{
			case ENET_EVENT_TYPE_CONNECT:
				AddConnectedUser(receivedEvent.peer);
			break;

			case ENET_EVENT_TYPE_DISCONNECT:
				RemoveConnectedUser(receivedEvent.peer);
			break;

			case ENET_EVENT_TYPE_NONE:
				std::cout << "Unknown event.\n";
			break;

			case ENET_EVENT_TYPE_RECEIVE:
				ReceivedPacket(&receivedEvent);
			break;

			default:
			break;
		}

		// done with the packet, destroy it
		enet_packet_destroy(receivedEvent.packet);
	}

	VerifyAndApplyPlayerData();
	UpdatePingValues();
	SendPlayerData();

	// send now to reduce latency?
	enet_host_flush(host);
}

void SNET_Server::ReceivedPacket(const ENetEvent* const event)
{
	switch((SNET_Channel)event->channelID)
	{
		case SNET_CHANNEL_CONNECTION:
			ReceivedConnectionPacket(event);
		break;
		case SNET_CHANNEL_ENTITY_DATA:
			ReceivedEntityPacket(event);
		break;
		case SNET_CHANNEL_LEADERBOARD:
			ReceivedLeaderboardPacket(event);
		break;
		default:
		break;
	}
}

void SNET_Server::ReceivedConnectionPacket(const ENetEvent* const event)
{
	const SNET_Packet_Connection* conPacket = (SNET_Packet_Connection*)event->packet->data;

	switch (conPacket->type)
	{
		// a newly connected player has sent us their userinfo
		case SNET_CONTYPE_USERINFO:
		{
			SNET_Packet_UserInfo* uinfoPacket = (SNET_Packet_UserInfo*)event->packet->data;
			SNET_ConnectedUser* user = connectedUsers[event->peer];
			// by now we already have the ID and peer, we just need to populate the username
			user->username = std::string(uinfoPacket->username);
			std::cout << "Received user data from ID " << user->id << "\n";
			// we now have all the data we need about the user, add them to the game
			AddPlayer(user);
		}
		break;

		default:
			break;
	}
}

void SNET_Server::ReceivedEntityPacket(const ENetEvent* const event)
{
	const SNET_Packet_EntityData* entPacket = (SNET_Packet_EntityData*)event->packet->data;

	switch (entPacket->type)
	{
		case SNET_ENTPACKET_ENTITY:

		break;

		case SNET_ENTPACKET_PLAYER:
			SNET_Packet_PlayerData* playerPacket = (SNET_Packet_PlayerData*)event->packet->data;
			SNET_NetworkedPlayer* player = players[playerPacket->id];
			player->AddNewPacketData(*playerPacket);
		break;

		default:
			break;
	}
}
