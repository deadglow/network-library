#pragma once
#include "enet/enet.h"
#include "SNET_Packets.h"
#include "SNET_IDPool.h"
#include <unordered_map>

class SNET_ConnectedUser;
class SNET_NetworkedPlayer;
class SNET_NetworkedEntity;
class SNET_Leaderboard;

class SNET_Server
{
private:
	ENetHost* host = nullptr;
	SNET_IDPool playerIDs { UINT16_MAX };
	SNET_IDPool entityIDs { UINT16_MAX };
	SNET_Leaderboard* leaderboard;
	std::unordered_map<ENetPeer*, SNET_ConnectedUser*> connectedUsers;
	std::unordered_map<UINT16, SNET_NetworkedPlayer*> players;
	std::unordered_map<UINT16, SNET_NetworkedEntity*> entities;

	void ReceivedPlayerPacket(ENetEvent* event);
	void ReceivedConnectionPacket(ENetEvent* event);
	void ReceivedLeaderboardPacket(ENetEvent* event);

	void AddConnectedUser(ENetPeer* const peer);
	void RemoveConnectedUser(ENetPeer* const peer);

	void AddPlayer(SNET_ConnectedUser* const user);
	void RemovePlayer(UINT16 id);

	void UpdatePingValues();
	void BroadcastData();
	void SendPlayerData();
	void VerifyAndApplyPlayerData();

public:
	SNET_Server();
	~SNET_Server();

	int Initialise();
	void Update();
	bool HasPlayerJoined(const UINT16 id);
	
	const int GetPlayerCount() const;
};