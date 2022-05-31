#pragma once
#include <unordered_map>
#include "SNET_UserDefined.h"

class SNET_Leaderboard
{
private:
    std::unordered_map<UINT16, SNET_LeaderboardEntry> entries;

public:
    void AddEntry(const UINT16 id);
    void DeleteEntry(const UINT16 id);

    SNET_LeaderboardEntry& GetEntry(const UINT16 id);

    bool EntryExists(const UINT16 id) const;
};