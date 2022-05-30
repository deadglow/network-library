#pragma once
#include "enet/enet.h"
#include "SNET_Packets.h"
#include "SNET_IDPool.h"
#include <unordered_map>

#include "SNET_Typedefs.h"

struct SNET_ConnectedUser;
class SNET_NetworkedPlayer;
class SNET_NetworkedEntity;
class SNET_Leaderboard;

class SNET_Server
{
private:
	std::string servername = "Server Name";
	ENetHost* host = nullptr;
	SNET_IDPool userIDs { UINT16_MAX };
	SNET_IDPool entityIDs { UINT16_MAX };
	SNET_Leaderboard* leaderboard;
	std::unordered_map<ENetPeer*, SNET_ConnectedUser*> connectedUsers;
	std::unordered_map<UINT16, SNET_NetworkedPlayer*> players;
	std::unordered_map<UINT16, SNET_NetworkedEntity*> entities;

	SNET_Func_PlayerConnection playerAddCallback = nullptr;
	SNET_Func_PlayerConnection playerRemoveCallback = nullptr;
	SNET_Func_ApplyPlayerData playerApplyDataCallback = nullptr;

	void ReceivedPacket(const ENetEvent* const event);
	void ReceivedConnectionPacket(const ENetEvent* const event);
	void ReceivedEntityPacket(const ENetEvent* const event);
	void ReceivedLeaderboardPacket(const ENetEvent* const event);

	void AddConnectedUser(ENetPeer* const peer);
	void RemoveConnectedUser(ENetPeer* const peer);

	void AddPlayer(const SNET_ConnectedUser* const user);
	void RemovePlayer(const UINT16 id);

	void UpdatePingValues();
	void BroadcastData();
	void VerifyData();

public:
	SNET_Server() = default;
	~SNET_Server();

	int Initialise();
	void SetCallbacks(SNET_Func_PlayerConnection onPlayerAdd, SNET_Func_PlayerConnection onPlayerRemove, SNET_Func_ApplyPlayerData onPlayerDataApply);

	void Update();
	
	bool HasPlayerJoined(const UINT16 id) const;
	
	const size_t GetPlayerCount() const;

	SNET_NetworkedPlayer* GetPlayer(const UINT16 id);
	SNET_NetworkedEntity* GetEntity(const UINT16 id);
};