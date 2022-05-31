#include "SNET_Client.h"
#include "SNET_NetworkedPlayer.h"
#include <iostream>

SNET_Client::~SNET_Client()
{
    if (connected)
        Disconnect();

    if (host != nullptr)
        enet_host_destroy(host);

    enet_deinitialize();
}

int SNET_Client::Initialise()
{
    if (enet_initialize() != 0)
        return EXIT_FAILURE;

    std::cout << "ENet initialised.\n";

    host = enet_host_create(nullptr, 1, SNET_CHANNEL_COUNT, 0, 0);

	if (host == NULL)
	{
		std::cout << "Failed to create host.\n";
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void SNET_Client::SetCallbacks(SNET_Func_PlayerConnection onPlayerAdd,
	SNET_Func_PlayerConnection onPlayerRemove,
	SNET_Func_ApplyPlayerData onApplyPlayerData,
	SNET_Func_GeneratePlayerData onGenPlayerData)
{
	playerAddCallback = onPlayerAdd;
	playerRemoveCallback = onPlayerRemove;
	playerApplyDataCallback = onApplyPlayerData;
	genPlayerDataCallback = onGenPlayerData;
}

bool SNET_Client::IsConnected() const
{
	return connected;
}

bool SNET_Client::IsInGame() const
{
	return inGame;
}

bool SNET_Client::Connect(const std::string ipAddress)
{
	if (connected)
		Disconnect();

	// read address
	ENetAddress address;
	address.port = SNET_DEFAULT_PORT;
	enet_address_set_host(&address, ipAddress.c_str());

	std::cout << "Attempting to connect to " << ipAddress << "...\n";
	serverPeer = enet_host_connect(host, &address, SNET_CHANNEL_COUNT, 0);

	ENetEvent connnectionEvent;
	if (enet_host_service(host, &connnectionEvent, SNET_CONNECTION_TIMEOUT) > 0 && connnectionEvent.type == ENET_EVENT_TYPE_CONNECT)
	{
		std::cout << "Connected to server! Waiting for server info...\n";
		connected = true;
	}
	else
	{
		std::cout << "Connection to server timed out.\n";
		enet_peer_reset(serverPeer);
	}

	return connected;
}

void SNET_Client::Disconnect()
{
	if (connected)
	{
		enet_peer_disconnect(serverPeer, 0);
		ENetEvent connectionEvent;
		if (enet_host_service(host, &connectionEvent, SNET_CONNECTION_TIMEOUT) > 0 && connectionEvent.type == ENET_EVENT_TYPE_DISCONNECT)
			std::cout << "Successfully disconnected from server.\n";
		else
		{
			std::cout << "Failed to properly disconnect from server. Forcing disconnection.\n";
			enet_peer_reset(serverPeer);
		}

		OnDisconnect();
	}
	else
		std::cout << "Not connected to a server.\n";
}

bool SNET_Client::PlayerExists(const UINT16 id) const
{
	return players.count(id) > 0;
}

void SNET_Client::OnDisconnect()
{
	// remove all players
	while (players.size() > 0)
		RemovePlayer(players.begin()->first);

	// clear leaderboard

	connected = false;
	inGame = false;
}

SNET_NetworkedPlayer* SNET_Client::GetPlayer(const UINT16 id)
{
	return players[id];
}

SNET_NetworkedEntity* SNET_Client::GetEntity(const UINT16 id)
{
	return entities[id];
}

UINT16 SNET_Client::GetClientID() const
{
	return clientID;
}

std::string SNET_Client::GetUsername() const
{
	return username;
}

void SNET_Client::SetUsername(const std::string name)
{
	username = name;
}

void SNET_Client::Update()
{
	// send player data to the server
	if (inGame)
	{
		SNET_Packet_PlayerData pdPacket = genPlayerDataCallback();
		pdPacket.id = clientID;
		pdPacket.type = SNET_ENTPACKET_PLAYER;

		// send to the server
		ENetPacket* packet = enet_packet_create(&pdPacket, sizeof(SNET_Packet_PlayerData), ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(serverPeer, SNET_CHANNEL_ENTITY_DATA, packet);
	}

	ENetEvent receivedEvent;
	// shuttle packets and receive data
	while (enet_host_service(host, &receivedEvent, 0) > 0)
	{
		switch (receivedEvent.type)
		{
			case ENET_EVENT_TYPE_CONNECT:
				std::cout << "This shouldn't appear!\n";
			break;

			case ENET_EVENT_TYPE_DISCONNECT:
			{
				if (connected)
				{
					std::cout << "Disconnected by server.\n";
					OnDisconnect();
				}
				else
					std::cout << "An error occured while trying to connect to the server.\n";
			}
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
		
		enet_packet_destroy(receivedEvent.packet);
	}
}

void SNET_Client::ReceivedPacket(const ENetEvent* const event)
{
	switch (event->channelID)
	{
		case SNET_CHANNEL_CONNECTION:
			ReceivedConnectionPacket(event->packet);
		break;

		case SNET_CHANNEL_ENTITY_DATA:
			ReceivedEntityPacket(event->packet);
		break;

		case SNET_CHANNEL_LEADERBOARD:
			ReceivedLeaderboardPacket(event->packet);
		break;

		default:
			break;
	}
}

void SNET_Client::ReceivedConnectionPacket(const ENetPacket* const packet)
{
	const SNET_Packet_Connection* conPacket = (SNET_Packet_Connection*)packet->data;

	switch (conPacket->type)
	{
		case SNET_CONTYPE_PLAYER_JOINED:
		{
			SNET_Packet_UserInfo* uinfoPacket = (SNET_Packet_UserInfo*)packet->data;
			AddPlayer(*uinfoPacket);
		}
		break;
		
		case SNET_CONTYPE_PLAYER_LEFT:
			RemovePlayer(conPacket->id);
		break;

		case SNET_CONTYPE_SERVERINFO:
		{
			SNET_Packet_ServerInfo* infoPacket = (SNET_Packet_ServerInfo*)packet->data;
			clientID = infoPacket->id;
			servername = std::string(infoPacket->servername);
			
			std::cout << "Connected to server: " << servername << ". Sending user information to server...\n";

			SNET_Packet_UserInfo newInfoPacket;
			newInfoPacket.type = SNET_CONTYPE_USERINFO;
			newInfoPacket.id = clientID;
			strcpy_s(newInfoPacket.username, username.c_str());

			ENetPacket* packet = enet_packet_create(&newInfoPacket, sizeof(SNET_Packet_UserInfo), ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send(serverPeer, SNET_CHANNEL_CONNECTION, packet);
		}
		break;

		default:
			break;
	}
}

void SNET_Client::ReceivedEntityPacket(const ENetPacket* const packet)
{
	if (!inGame) return;

	// figure out how many player data were received
	UINT16 playerCount = *(UINT16*)packet->data;

	// data pointer to the memory after the player count
	char* dataPos = (char*)packet->data + sizeof(UINT16);

	for (int i = 0; i < playerCount; ++i)
	{
		SNET_Packet_PlayerData* playerData = (SNET_Packet_PlayerData*)dataPos;
		dataPos += sizeof(SNET_Packet_PlayerData);

		// continue if the information is about the current player
		if (playerData->id == clientID) continue;

		if (players.count(playerData->id) == 0)
		{
			std::cout << "Player doesn't exist.\n";
			continue;
		}

		// copy player data to the networked player
		players[playerData->id]->PushPacketToHistory(*playerData);
	}
}

void SNET_Client::ReceivedLeaderboardPacket(const ENetPacket* const packet)
{
	if (!inGame) return;

	leaderboard.clear();

	UINT16 playerCount = *(UINT16*)packet->data;

	char* dataPos = (char*)packet->data + sizeof(UINT16);

	// populate the leaderboard
	for (int i = 0; i < playerCount; ++i)
	{
		// create leaderboard entry
		SNET_LeaderboardEntry* entry = (SNET_LeaderboardEntry*)dataPos;
		// offset datapos
		dataPos += sizeof(SNET_LeaderboardEntry);

		// ignore if the entry is not present in the players list and is not the client's id
		if (players.count(entry->id) == 0 && entry->id != clientID) continue;

		// push back to the leaderboard vector
		leaderboard.push_back(*entry);
	}
}

void SNET_Client::AddPlayer(SNET_Packet_UserInfo userInfo)
{
	// the peer will be told about itself first, inform the player that they've joined
	if (userInfo.id == clientID)
	{
		if (!inGame)
		{
			inGame = true;
			std::cout << "Successfully joined the game.\n";
		}

		return;
	}

	if (players.count(userInfo.id) > 0)
	{
		std::cout << "Player " << userInfo.id << " already exists.\n";
		return;
	}

	// create the new player
	std::string username = std::string(userInfo.username);
	SNET_NetworkedPlayer* newPlayer = new SNET_NetworkedPlayer(userInfo.id, userInfo.username);
	players.emplace(userInfo.id, newPlayer);

	// perform callback
	playerAddCallback(userInfo.id);

	std::cout << "Player (" << userInfo.id << ") " << username << " has joined.\n";
}

void SNET_Client::RemovePlayer(const UINT16 id)
{
	if (players.count(id) == 0)
	{
		std::cout << "Player with ID: " << id << " doesn't exist.\n";
		return;
	}

	std::string deletedName = players[id]->GetUsername();
	delete players[id];
	players.erase(id);

	std::cout << "Player (" << id << ") " << deletedName << " has left.\n";
}

void SNET_Client::RequestLeaderboard()
{
	ENetPacket* packet = enet_packet_create(NULL, 0, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(serverPeer, SNET_CHANNEL_LEADERBOARD, packet);
}

std::vector<SNET_LeaderboardEntry>& SNET_Client::GetLeaderboard()
{
	return leaderboard;
}