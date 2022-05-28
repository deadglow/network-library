#pragma once
#include <vector>
#include <queue>

class SNET_IDPool
{
	int maxIDs = 0;
	int IDcount = 0;
	int usedIDs = 0;
	std::queue<int> freeIDs;

public:
	SNET_IDPool(const int maxValue_init);
	~SNET_IDPool() = default;

	const int GetNewID();
	void ClearID(const int id);

	const int GetUsedIDCount() const;
};