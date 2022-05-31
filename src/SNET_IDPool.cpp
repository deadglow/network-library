#include "SNET_IDPool.h"

SNET_IDPool::SNET_IDPool(const int maxValue_init) : maxIDs(maxValue_init)
{
	// reserve the 0th id
	GetNewID();
}

const int SNET_IDPool::GetNewID()
{
	// return -1 if it failed to get a new ID
	int val = -1;
	// take from the queue if there are any spare
	if (freeIDs.size() > 0)
	{
		val = freeIDs.front();
		freeIDs.pop();
		usedIDs++;
	}
	else
	{
		// otherwise create a new ID
		if (IDcount < maxIDs)
		{
			val = IDcount;
			IDcount++;
			usedIDs++;
		}
	}

	return val;
}

void SNET_IDPool::ClearID(const int id)
{
	if (id == 0)
		throw "Can't clear 0th id.";
	// pass the used ID to the freeID queue
	freeIDs.push(id);
	usedIDs--;
}

const int SNET_IDPool::GetUsedIDCount() const
{
	return usedIDs;
}