#pragma once
#include "SNET_BasePackets.h"

struct SNET_Packet_PlayerData
	: public SNET_Packet_EntityData
{
	SNET_Vec2 viewAngles;
	int extra;
};

struct SNET_LeaderboardEntry
	: public SNET_LeaderboardEntry_Base
{
	UINT16 kills;
	UINT16 deaths;
};