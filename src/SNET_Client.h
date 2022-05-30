#pragma once
#include "enet/enet.h"
#include "SNET_Packets.h"
#include <vector>
#include <unordered_map>

class SNET_NetworkedPlayer;
class SNET_NetworkedEntity;

class SNET_Client
{
private:
	std::string username = "Player";
	UINT16 clientID = 0;
	std::string serverIP;
	std::string servername = "";
	
	bool connected = false;
	bool joinedGame = false;

	ENetHost* host = nullptr;
	ENetPeer* serverPeer = nullptr;

	SNET_Func_PlayerConnection playerAddCallback = nullptr;
	SNET_Func_PlayerConnection playerRemoveCallback = nullptr;
	SNET_Func_ApplyPlayerData playerApplyDataCallback = nullptr;

	// leaderboard
	std::unordered_map<UINT16, SNET_NetworkedPlayer*> players;
	std::unordered_map<UINT16, SNET_NetworkedEntity*> entities;

	void ReceivedConnectionPacket(const ENetPacket* const packet);
	void ReceivedEntityPacket(const ENetPacket* const packet);
	void ReceivedLeaderboardPacket(const ENetPacket* const packet);
	
	void OnDisconnect();

	void AddPlayer(const UINT16 id, const std::string playerUsername);
	void RemovePlayer(const UINT16 id);

public:
	SNET_Client() = default;
	~SNET_Client();

	int Initialise();
	void SetCallbacks(SNET_Func_PlayerConnection onPlayerAdd, SNET_Func_PlayerConnection onPlayerRemove, SNET_Func_ApplyPlayerData onApplyPlayerData);
	
	bool Connect(const std::string ipAddress);
	void Disconnect();

	bool IsConnected();

	SNET_NetworkedPlayer* GetPlayer(const UINT16 id);
	SNET_NetworkedEntity* GetEntity(const UINT16 id);

	void Update();

	void RequestLeaderboard();
};