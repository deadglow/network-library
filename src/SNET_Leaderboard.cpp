#include "SNET_Leaderboard.h"


void SNET_Leaderboard::AddEntry(const UINT16 id)
{
    SNET_LeaderboardEntry entry;
    entry.id = id;
    entries.emplace(id, entry);
}

void SNET_Leaderboard::DeleteEntry(const UINT16 id)
{
    entries.erase(id);
}

SNET_LeaderboardEntry& SNET_Leaderboard::GetEntry(const UINT16 id)
{
    return entries[id];
}

bool SNET_Leaderboard::EntryExists(const UINT16 id) const
{
    return entries.count(id) > 0;
}

