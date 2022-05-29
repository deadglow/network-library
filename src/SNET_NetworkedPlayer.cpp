#include "SNET_NetworkedPlayer.h"
SNET_NetworkedPlayer::SNET_NetworkedPlayer(const UINT16 id_init, const std::string username_init)
	: id(id_init), username(username_init), packetDataHistory(SNET_PLAYER_DATA_HISTORY_SIZE)
{
	// create player here
}

SNET_NetworkedPlayer::~SNET_NetworkedPlayer()
{
	// remove player here
}

UINT16 SNET_NetworkedPlayer::GetID() const
{
	return id;
}

std::string SNET_NetworkedPlayer::GetUsername() const
{
	return username;
}

void SNET_NetworkedPlayer::PushPacketToHistory(const SNET_Packet_PlayerData packet)
{
	packetDataHistory.push_back(packet);
}

SNET_Packet_PlayerData SNET_NetworkedPlayer::GetPacket(const int offset)
{
	return packetDataHistory.get_at_offset(offset);
}

SNET_Packet_PlayerData SNET_NetworkedPlayer::GetInterpolatedPacket(const float t)
{
	SNET_Packet_PlayerData newPacket;
	if (packetDataHistory.size() == 0)
		return newPacket;

	SNET_Packet_PlayerData latestPacket = packetDataHistory.get_at_offset(0);
	SNET_Packet_PlayerData latestPacket2 = (packetDataHistory.size() > 1)? packetDataHistory.get_at_offset(1) : latestPacket;
	
	// interpolate packet here

	return newPacket;
}