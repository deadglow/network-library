#include "enet/enet.h"
#include <string>

#define SNET_SERVERNAME_MAX 32
#define SNET_USERNAME_MAX 32

#define SNET_DEFAULT_PORT 3453
#define SNET_MAX_CONNECTIONS 16
#define SNET_CONNECTION_TIMEOUT 5000
#define SNET_PLAYER_DATA_HISTORY_SIZE 5
#define SNET_ENTITY_DATA_HISTORY_SIZE 3

// --------------------------------------------
//		ENUMS
// -----------------------------------------/

enum SNET_Channel : int
{
	SNET_CHANNEL_CONNECTION,
	SNET_CHANNEL_ENTITY_DATA,
	SNET_CHANNEL_LEADERBOARD,
	SNET_CHANNEL_MESSAGE,
	SNET_CHANNEL_EVENT,
	SNET_CHANNEL_COUNT
};

enum SNET_ConnectionType : UINT16
{
	SNET_CONTYPE_PLAYER_JOINED,
	SNET_CONTYPE_PLAYER_LEFT,
	SNET_CONTYPE_SERVERINFO,
	SNET_CONTYPE_USERINFO,
	SNET_CONTYPE_COUNT
};

enum SNET_EntityPacketType : UINT8
{
	SNET_ENTPACKET_ENTITY,
	SNET_ENTPACKET_PLAYER
};

// -------------------------------------------
//		STRUCTS
// -----------------------------------------/

struct SNET_Vec2
{
	float x;
	float y;
};

struct SNET_Vec3
	: public SNET_Vec2
{
	float z;
};

struct SNET_Vec4
	: public SNET_Vec3
{
	float w;
};

struct SNET_ConnectedUser
{
	UINT16 id;
	ENetPeer* peer;
	std::string username;
	std::string ip;
};