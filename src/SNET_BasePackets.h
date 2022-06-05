#pragma once
#include "SNET_Datatypes.h"

struct SNET_Packet_Connection
{
	SNET_ConnectionType type;
	UINT16 id;
};

struct SNET_Packet_UserInfo
	: public SNET_Packet_Connection
{
	char username[SNET_USERNAME_MAX];
};

struct SNET_Packet_ServerInfo
	: public SNET_Packet_Connection
{
	char servername[SNET_SERVERNAME_MAX];
};

struct SNET_Packet_EntityData
{
	SNET_EntityPacketType type;
	UINT16 id = 0;
	SNET_Vec3 position;
	SNET_Quat rotation;
};

struct SNET_LeaderboardEntry_Base
{
	UINT16 id = 0;
	UINT16 ping = 0;
};