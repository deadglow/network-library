#include "SNET_Server.h"
#include "SNET_NetworkedPlayer.h"
#include "SNET_Leaderboard.h"
#include <iostream>

SNET_Server::~SNET_Server()
{
	// remove all players
	while (connectedUsers.size() > 0)
		RemoveConnectedUser(connectedUsers.begin()->first);

	if (host)
		enet_host_destroy(host);

	if (leaderboard)
		delete leaderboard;

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

	leaderboard = new SNET_Leaderboard();

	return EXIT_SUCCESS;
}

void SNET_Server::SetCallbacks(SNET_Func_PlayerConnection onPlayerAdd, SNET_Func_PlayerConnection onPlayerRemove, SNET_Func_ApplyPlayerData onPlayerDataApply)
{
	playerAddCallback = onPlayerAdd;
	playerRemoveCallback = onPlayerRemove;
	playerApplyDataCallback = onPlayerDataApply;
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

	UpdatePingValues();
	VerifyData();
	BroadcastData();

	// send now to reduce latency?
	enet_host_flush(host);
}

bool SNET_Server::HasPlayerJoined(const UINT16 id) const
{
	return players.count(id) > 0;
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
		{
			SNET_Packet_PlayerData* playerPacket = (SNET_Packet_PlayerData*)event->packet->data;
			SNET_NetworkedPlayer* player = players[playerPacket->id];
			player->PushPacketToHistory(*playerPacket);
		}
		break;

		default:
			break;
	}
}

void SNET_Server::ReceivedLeaderboardPacket(const ENetEvent* const event)
{
	// figure out the total packet size
	UINT16 playerCount = players.size();
	size_t totalSize = sizeof(UINT16) + sizeof(SNET_LeaderboardEntry) * playerCount;

	// gen a data pointer
	char* data = new char[totalSize];
	memcpy_s(data, sizeof(UINT16), &playerCount, sizeof(UINT16));

	char* dataPos = data + sizeof(UINT16);
	for (const auto [id, player] : players)
	{
		SNET_LeaderboardEntry& entry = leaderboard->GetEntry(id);
		memcpy_s(dataPos, sizeof(SNET_LeaderboardEntry), &entry, sizeof(SNET_LeaderboardEntry));
		dataPos += sizeof(SNET_LeaderboardEntry);
	}

	// create and send the packet
	ENetPacket* packet = enet_packet_create(data, totalSize, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(event->peer, SNET_CHANNEL_LEADERBOARD, packet);

	delete[] data;
}

void SNET_Server::AddConnectedUser(ENetPeer* const peer)
{
	// create a new connected user and give it a new ID
	SNET_ConnectedUser* user = new SNET_ConnectedUser();
	UINT16 newUserID = userIDs.GetNewID();
	user->id = newUserID;
	user->peer = peer;
	connectedUsers.emplace(user->peer, user);

	// tell the new peer of its new ID and other server data
	SNET_Packet_ServerInfo infoPacket;
	infoPacket.type = SNET_CONTYPE_SERVERINFO;
	infoPacket.id = newUserID;
	strcpy_s(infoPacket.servername, servername.c_str());

	ENetPacket* packet = enet_packet_create(&infoPacket, sizeof(SNET_Packet_ServerInfo), ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, SNET_CHANNEL_CONNECTION, packet);

	std::cout << "Peer " << peer->address.host << " connected.\n";
}

void SNET_Server::RemoveConnectedUser(ENetPeer* const peer)
{
	SNET_ConnectedUser* user = connectedUsers[peer];
	UINT16 id = user->id;

	// remove the player if they are in the game
	if (HasPlayerJoined(id))
		RemovePlayer(id);
	else
		std::cout << "User has not joined the game, continuing to disconnect.\n";
	
	delete user;
	connectedUsers.erase(peer);

	userIDs.ClearID(id);

	std::cout << "Peer " << peer->address.host << " disconnected.\n";
}

void SNET_Server::AddPlayer(const SNET_ConnectedUser* const user)
{
	// create the new player
	SNET_NetworkedPlayer* newPlayer = new SNET_NetworkedPlayer(user->id, user->username);
	players.emplace(user->id, newPlayer);
	
	// do callback
	playerAddCallback(user->id);
	
	// include in leaderboard
	leaderboard->AddEntry(user->id);
	SNET_LeaderboardEntry& entry = leaderboard->GetEntry(user->id);
	strcpy_s(entry.username, user->username.c_str());

	// tell existing players of the new player's existence
	// generate the info packet
	SNET_Packet_UserInfo info;
	info.id = user->id;
	info.type = SNET_CONTYPE_PLAYER_JOINED;
	strcpy_s(info.username, user->username.c_str());

	// broadcast to all peers
	ENetPacket* newPlayerPacket = enet_packet_create(&info, sizeof(SNET_Packet_UserInfo), ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(host, SNET_CHANNEL_CONNECTION, newPlayerPacket);

	// inform the new user of the players that have already joined
	for (const auto [id, player] : players)
	{
		// generate packet for the current player in the loop
		SNET_Packet_UserInfo thisPlayer;
		thisPlayer.id = player->GetID();
		thisPlayer.type = SNET_CONTYPE_PLAYER_JOINED;
		strcpy_s(thisPlayer.username, player->GetUsername().c_str());

		ENetPacket* packet = enet_packet_create(&thisPlayer, sizeof(SNET_Packet_UserInfo), ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(user->peer, SNET_CHANNEL_CONNECTION, packet);
	}

	std::cout << "Player (" << user->id << ") " << user->username << " has joined.\n";
}

void SNET_Server::RemovePlayer(const UINT16 id)
{
	if (players.count(id) == 0) throw std::out_of_range("Attempting to remove a player that doesn't exist.");

	// do callback
	playerRemoveCallback(id);

	SNET_NetworkedPlayer* player = players[id];
	std::cout << "Player (" << id << ") " << player->GetUsername() << " has left.\n";

	// delete the player from the player list
	delete player;
	players.erase(id);
	//remove player from leaderboard
	leaderboard->DeleteEntry(id);

	// inform all users to delete that player
	SNET_Packet_Connection conPacket;
	conPacket.id = id;
	conPacket.type = SNET_CONTYPE_PLAYER_LEFT;

	// create the packet to inform the users that a player has left
	ENetPacket* playerLeftPacket = enet_packet_create(&conPacket, sizeof(SNET_Packet_Connection), ENET_PACKET_FLAG_RELIABLE);

	enet_host_broadcast(host, SNET_CHANNEL_CONNECTION, playerLeftPacket);
}

void SNET_Server::UpdatePingValues()
{
	for (const auto [peer, user] : connectedUsers)
	{
		// skip if the player hasn't joined
		if (!HasPlayerJoined(user->id)) continue;

		// update ping value here
		leaderboard->GetEntry(user->id).ping = peer->roundTripTime;
	}
}

// read latest player data and create a packet using the data
void SNET_Server::BroadcastData()
{
	// get player count
	UINT16 playerCount = (UINT16)players.size();
	size_t totalSize = sizeof(UINT16) + sizeof(SNET_EntityPacketType) + sizeof(SNET_Packet_PlayerData) * playerCount;

	char* totalData = new char[totalSize]();
	char* dataPos = totalData;

	// copy in the number of players
	memcpy_s(dataPos, sizeof(UINT16), &playerCount, sizeof(UINT16));
	dataPos += sizeof(UINT16);

	// copy in the type of packet
	SNET_EntityPacketType packetType = SNET_ENTPACKET_PLAYER;
	memcpy_s(dataPos, sizeof(SNET_EntityPacketType), &packetType, sizeof(SNET_EntityPacketType));
	dataPos += sizeof(SNET_EntityPacketType);

	// default data in case the player doesn't have existing data
	SNET_Packet_PlayerData defaultData;
	defaultData.type = SNET_ENTPACKET_PLAYER;

	// copy in each player's data
	for (const auto [id, player] : players)
	{
		defaultData.id = player->GetID();
		SNET_Packet_PlayerData* usedPacket = (player->GetPacketCount() == 0)? &defaultData : &player->GetPacket(0);
		memcpy_s(dataPos, sizeof(SNET_Packet_PlayerData), usedPacket, sizeof(SNET_Packet_PlayerData));
		dataPos += sizeof(SNET_Packet_PlayerData);
	}

	// broadcast the data
	ENetPacket* packet = enet_packet_create(totalData, totalSize, ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(host, SNET_CHANNEL_ENTITY_DATA, packet);

	delete[] totalData;
}

void SNET_Server::VerifyData()
{
	for (const auto [id, player] : players)
	{
		// callback to verify and apply the data to the player
		playerApplyDataCallback(player);
	}
}

const size_t SNET_Server::GetPlayerCount() const
{
	return players.size();
}

SNET_NetworkedPlayer* SNET_Server::GetPlayer(const UINT16 id)
{
	return players[id];
}

SNET_NetworkedEntity* SNET_Server::GetEntity(const UINT16 id)
{
	return entities[id];
}