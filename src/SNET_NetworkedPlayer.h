#pragma once
#include "SNET_Definitions.h"
#include "SNET_RingBuffer.h"

class SNET_NetworkedPlayer
{
private:
	UINT16 id;
	std::string username;
	SNET_RingBuffer<SNET_Packet_PlayerData> packetDataHistory;

public:
	SNET_NetworkedPlayer(const UINT16 id_init, const std::string username_init);
	~SNET_NetworkedPlayer();

	UINT16 GetID() const;
	std::string GetUsername() const;

	// Adds a player data packet to the front of the cyclic packet history
	void PushPacketToHistory(const SNET_Packet_PlayerData packet);

	// Number of packets added to the cyclic packet history
	const size_t GetPacketCount() const;

	// Gets the Nth packet before the latest packet
	SNET_Packet_PlayerData GetPacket(const int offset);

	// Override the Nth packet before the latest packet
	void OverridePacket(const int offset, const SNET_Packet_PlayerData& packet);
};