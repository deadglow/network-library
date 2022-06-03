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

// MATHS

struct SNET_Vec2
{
	float x = 0;
	float y = 0;

	SNET_Vec2() = default;
	SNET_Vec2(const float x_init, const float y_init)
		: x(x_init), y(y_init) {};

	SNET_Vec2 Normalize()
	{
		float magnitude = sqrtf(x * x + y * y);
		return SNET_Vec2(x / magnitude, y / magnitude);
	}
};

struct SNET_Vec3
	: public SNET_Vec2
{
	float z = 0;

	SNET_Vec3() = default;
	SNET_Vec3(const float x_init, const float y_init, const float z_init)
		: SNET_Vec2(x_init, y_init), z(z_init) {};

	SNET_Vec3 Normalize()
	{
		float magnitude = sqrtf(x * x + y * y + z * z);
		return SNET_Vec3(x / magnitude, y / magnitude, z / magnitude);
	}
};

struct SNET_Vec4
	: public SNET_Vec3
{
	float w = 0;

	SNET_Vec4() = default;
	SNET_Vec4(const float x_init, const float y_init, const float z_init, const float w_init)
		: SNET_Vec3(x_init, y_init, z_init), w(w_init) {};

	SNET_Vec4 Normalize()
	{
		float magnitude = sqrtf(x * x + y * y + z * z + w * w);
		return SNET_Vec4(x / magnitude, y / magnitude, z / magnitude, w / magnitude);
	}
};

struct SNET_Quat
	: public SNET_Vec4
{
	SNET_Quat()
		: SNET_Vec4(0.0f, 0.0f, 0.0f, 1.0f) {};
	SNET_Quat(const float x_init, const float y_init, const float z_init, const float w_init)
		: SNET_Vec4(x_init, y_init, z_init, w_init) {};

	SNET_Quat Normalize()
	{
		float magnitude = sqrtf(x * x + y * y + z * z + w * w);
		return SNET_Quat(x / magnitude, y / magnitude, z / magnitude, w / magnitude);
	}
};

// OTHER

struct SNET_ConnectedUser
{
	UINT16 id;
	ENetPeer* peer;
	std::string username;
	std::string ip;
	// number of frames since last packed received
	unsigned short packetDelay;
};