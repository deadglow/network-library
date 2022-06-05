#pragma once
#include "SNET_Packets.h"

class SNET_NetworkedPlayer;

typedef void (* SNET_Func_PlayerConnection)(const UINT16 id);
typedef void (* SNET_Func_ApplyPlayerData)(SNET_NetworkedPlayer* player);
typedef SNET_Packet_PlayerData (* SNET_Func_GeneratePlayerData)();