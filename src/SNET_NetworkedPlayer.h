#pragma once
#include "SNET_Packets.h"
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

	void PushPacketToHistory(const SNET_Packet_PlayerData packet);

	SNET_Packet_PlayerData GetPacket(const int offset);

	SNET_Packet_PlayerData GetInterpolatedPacket(const float t);
};