#pragma once

#include "Define.h"
#include "SharedDefines.h"

#include <vector>
#include <sstream>
#include <string>

inline void split(std::vector<std::string>& dest, std::string const str, char const* delim)
{
    char* pTempStr = strdup(str.c_str());
    char* pWord = strtok(pTempStr, delim);

    while (pWord != nullptr)
    {
        dest.push_back(pWord);
        pWord = strtok(nullptr, delim);
    }

    free(pTempStr);
}

inline std::vector<std::string>& split(std::string const s, char delim, std::vector<std::string>& elems)
{
    std::stringstream ss(s);
    std::string item;

    while (getline(ss, item, delim))
    {
        elems.push_back(item);
    }

    return elems;
}

inline std::vector<std::string> split(std::string const s, char delim)
{
    std::vector<std::string> elems;
    return split(s, delim, elems);
}

static bool IsAlliance(uint8 race)
{
    return race == RACE_HUMAN || race == RACE_DWARF || race == RACE_NIGHTELF || race == RACE_GNOME ||
        race == RACE_DRAENEI || race == RACE_WORGEN || race == RACE_PANDAREN_ALLIANCE;
}

inline std::string ClassToString(Classes cls)
{
    switch (cls)
    {
    case CLASS_WARRIOR:
        return "CLASS_WARRIOR";
    case CLASS_PALADIN:
        return "CLASS_PALADIN";
    case CLASS_HUNTER:
        return "CLASS_HUNTER";
    case CLASS_ROGUE:
        return "CLASS_ROGUE";
    case CLASS_PRIEST:
        return "CLASS_PRIEST";
    case CLASS_DEATH_KNIGHT:
        return "CLASS_DEATH_KNIGHT";
    case CLASS_SHAMAN:
        return "CLASS_SHAMAN";
    case CLASS_MAGE:
        return "CLASS_MAGE";
    case CLASS_WARLOCK:
        return "CLASS_WARLOCK";
    case CLASS_MONK:
        return "CLASS_MONK";
    case CLASS_DRUID:
        return "CLASS_DRUID";
    }

    return "Undefined";
}

inline std::string RaceToString(Races race)
{
    switch (race)
    {
    case RACE_NONE:
        return "RACE_NONE";
    case RACE_HUMAN:
        return "RACE_HUMAN";
    case RACE_ORC:
        return "RACE_ORC";
    case RACE_DWARF:
        return "RACE_DWARF";
    case RACE_NIGHTELF:
        return "RACE_NIGHTELF";
    case RACE_UNDEAD_PLAYER:
        return "RACE_UNDEAD_PLAYER";
    case RACE_TAUREN:
        return "RACE_TAUREN";
    case RACE_GNOME:
        return "RACE_GNOME";
    case RACE_TROLL:
        return "RACE_TROLL";
    case RACE_GOBLIN:
        return "RACE_GOBLIN";
    case RACE_BLOODELF:
        return "RACE_BLOODELF";
    case RACE_DRAENEI:
        return "RACE_DRAENEI";
    case RACE_WORGEN:
        return "RACE_WORGEN";
    case RACE_PANDAREN_NEUTRAL:
        return "RACE_PANDAREN_NEUTRAL";
    case RACE_PANDAREN_ALLIANCE:
        return "RACE_PANDAREN_ALLIANCE";
    case RACE_PANDAREN_HORDE:
        return "RACE_PANDAREN_HORDE";
    default:
        return "Undefined";
    }
}
