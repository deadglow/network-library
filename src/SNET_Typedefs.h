#pragma once

class SNET_NetworkedPlayer;

typedef void (* SNET_Func_PlayerConnection)(const UINT16 id);
typedef void (* SNET_Func_ApplyPlayerData)(SNET_NetworkedPlayer* player);