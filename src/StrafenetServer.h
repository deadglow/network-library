#include "enet/enet.h"
#include <map>

struct PlayerPacket
{
	int playerID = -1;
	float position[3] {0.0f, 0.0f, 0.0f};
	float velocity[3] {0.0f, 0.0f, 0.0f};
	float viewAngle[2] {0.0f, 0.0f};
};

class StrafeNetServer
{
private:
	std::map<int, PlayerPacket> playerDataMap;
	ENetHost* host = nullptr;

public:
	StrafeNetServer() = default;
	~StrafeNetServer() = default;

	int Initialise();
	
	void AddPlayer(const int id);
	void RemovePlayer(const int id);
	const int GetPlayerCount() const;

	void SendData();
	void ReceiveData();
};