/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#ifndef _PLAYERBOT_RANDOMPLAYERBOTMGR_H
#define _PLAYERBOT_RANDOMPLAYERBOTMGR_H

#include "PlayerbotMgr.h"

class Player;
class RandomPlayerbotMgr : public PlayerbotHolder
{
public:
    RandomPlayerbotMgr();
    virtual ~RandomPlayerbotMgr();
    static RandomPlayerbotMgr* instance()
    {
        static RandomPlayerbotMgr instance;
        return &instance;
    }

    void UpdateAIInternal(uint32 elapsed, bool minimal = false) override;

private:
    //void ScaleBotActivity();

public:
    uint32 activeBots = 0;
    bool IsRandomBot(Player* bot);
    bool IsRandomBot(uint64 bot);
    void Clear(Player* bot);
    void OnPlayerLogout(Player* player);
    void OnPlayerLogin(Player* player);
    void OnPlayerLoginError(uint64 bot);
    Player* GetRandomPlayer();
    std::vector<Player*> GetPlayers() { return players; };
    PlayerBotMap GetAllBots() { return playerBots; };
    void Refresh(Player* bot);
    uint32 GetMaxAllowedBotCount();
    bool ProcessBot(Player* player);
    void Revive(Player* player);
    uint32 GetValue(Player* bot, std::string const type);
    uint32 GetValue(uint64 bot, std::string const type);
    std::string const GetData(uint32 bot, std::string const type);
    void SetValue(uint64 bot, std::string const type, uint32 value, std::string const data = "");
    void SetValue(Player* bot, std::string const type, uint32 value, std::string const data = "");
    void Remove(Player* bot);
    void CheckPlayers();

    float getActivityMod() { return activityMod; }
    float getActivityPercentage() { return activityMod * 100.0f; }
    void setActivityPercentage(float percentage) { activityMod = percentage / 100.0f; }
    static uint8 GetTeamClassIdx(bool isAlliance, uint8 claz) { return isAlliance * 20 + claz; }

protected:
    void OnBotLoginInternal(Player* const bot) override;

private:
    // pid values are set in constructor
    float activityMod = 0.25;
    bool _isBotInitializing = true;
    uint32 GetEventValue(uint64 bot, std::string const event);
    std::string const GetEventData(uint64 bot, std::string const event);
    uint32 SetEventValue(uint64 bot, std::string const event, uint32 value, uint32 validIn,
        std::string const data = "");
    void GetBots();

    time_t PlayersCheckTimer;

    uint32 AddRandomBots();
    bool ProcessBot(uint64 bot);

    typedef void (RandomPlayerbotMgr::* ConsoleCommandHandler)(Player*);

    std::vector<Player*> players;
    uint32 processTicks;

    std::list<uint64> currentBots;
    uint32 playersLevel;
};

#define sRandomPlayerbotMgr RandomPlayerbotMgr::instance()

#endif
