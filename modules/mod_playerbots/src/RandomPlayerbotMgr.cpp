/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "RandomPlayerbotMgr.h"

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <random>

#include "AccountMgr.h"
#include "ArenaTeamMgr.h"
#include "Battleground.h"
#include "BattlegroundMgr.h"
#include "CellImpl.h"
#include "ChannelMgr.h"
#include "DBCStores.h"
#include "DBCStructure.h"
#include "DatabaseEnv.h"
#include "Define.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "GuildMgr.h"
#include "LFGMgr.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"
#include "SharedDefines.h"
#include "Unit.h"
#include "World.h"

RandomPlayerbotMgr::RandomPlayerbotMgr() : PlayerbotHolder(), processTicks(0)
{
    playersLevel = 1;// sPlayerbotAIConfig->randombotStartingLevel;
    PlayersCheckTimer = 0;
}

RandomPlayerbotMgr::~RandomPlayerbotMgr() {}

uint32 RandomPlayerbotMgr::GetMaxAllowedBotCount() { return GetEventValue(0, "bot_count"); }

void RandomPlayerbotMgr::UpdateAIInternal(uint32 elapsed, bool /*minimal*/)
{
}

uint32 RandomPlayerbotMgr::AddRandomBots()
{
    return currentBots.size();
}

void RandomPlayerbotMgr::CheckPlayers()
{
}

bool RandomPlayerbotMgr::ProcessBot(uint64 bot)
{
    Player* player = GetPlayerBot(bot);
    PlayerbotAI* botAI = player ? GET_PLAYERBOT_AI(player) : nullptr;

    uint32 isValid = GetEventValue(bot, "add");
    if (!isValid)
    {
        if (!player || !player->GetGroup())
        {
            if (player)
            {
                auto side = (player->GetTeamId() == TeamId::TEAM_ALLIANCE ? "A" : "H");
                SF_LOG_INFO("playerbots", "Bot #%u %s:%u <%s>: log out", bot, side, player->getLevel(), player->GetName().c_str());
            }
            else
                SF_LOG_INFO("playerbots", "Bot #%u: log out", bot);

            SetEventValue(bot, "add", 0, 0);
            currentBots.erase(std::remove(currentBots.begin(), currentBots.end(), bot), currentBots.end());

            if (player)
                LogoutPlayerBot(bot);
        }

        return false;
    }

    uint32 randomTime;
    if (!player)
    {
        AddPlayerBot(bot, 0);
        /*randomTime = urand(1, 2);

        uint32 randomBotUpdateInterval = _isBotInitializing ? 1 : sPlayerbotAIConfig->randomBotUpdateInterval;
        randomTime = urand(std::max(5, static_cast<int>(randomBotUpdateInterval * 0.5)),
            std::max(12, static_cast<int>(randomBotUpdateInterval * 2)));
        SetEventValue(bot, "update", 1, randomTime);

        // do not randomize or teleport immediately after server start (prevent lagging)
        if (!GetEventValue(bot, "randomize"))
        {
            randomTime = urand(3, std::max(4, static_cast<int>(randomBotUpdateInterval * 0.4)));
            ScheduleRandomize(bot, randomTime);
        }
        if (!GetEventValue(bot, "teleport"))
        {
            randomTime = urand(std::max(7, static_cast<int>(randomBotUpdateInterval * 0.7)),
                std::max(14, static_cast<int>(randomBotUpdateInterval * 1.4)));
            ScheduleTeleport(bot, randomTime);
        }*/

        return true;
    }

    if (!player->IsInWorld())
        return false;

    if (player->GetGroup() || player->HasUnitState(UNIT_STATE_IN_FLIGHT))
        return false;

    uint32 update = GetEventValue(bot, "update");
    if (!update)
    {
        //if (botAI)
            //botAI->GetAiObjectContext()->GetValue<bool>("random bot update")->Set(true);

        bool update = true;
        if (botAI)
        {
            // botAI->GetAiObjectContext()->GetValue<bool>("random bot update")->Set(true);
            if (!sRandomPlayerbotMgr->IsRandomBot(player))
                update = false;

            if (player->GetGroup()/* && botAI->GetGroupMaster()*/)
            {
                //PlayerbotAI* groupMasterBotAI = GET_PLAYERBOT_AI(botAI->GetGroupMaster());
                //if (!groupMasterBotAI || groupMasterBotAI->IsRealPlayer())
                //{
                //    update = false;
                //}
            }
        }

        if (update)
            ProcessBot(player);

        //randomTime = urand(sPlayerbotAIConfig->minRandomBotReviveTime, sPlayerbotAIConfig->maxRandomBotReviveTime);
        //SetEventValue(bot, "update", 1, randomTime);

        return true;
    }

    uint32 logout = GetEventValue(bot, "logout");
    if (player && !logout && !isValid)
    {
        auto side = (player->GetTeamId() == TeamId::TEAM_ALLIANCE ? "A" : "H");
        SF_LOG_INFO("playerbots", "Bot #%u %s:%u <%s>: log out", bot, side, player->getLevel(), player->GetName().c_str());
        LogoutPlayerBot(bot);
        currentBots.remove(bot);
        //SetEventValue(bot, "logout", 1, urand(sPlayerbotAIConfig->minRandomBotInWorldTime, sPlayerbotAIConfig->maxRandomBotInWorldTime));
        return true;
    }

    return false;
}

bool RandomPlayerbotMgr::ProcessBot(Player* player)
{
    uint64 bot = player->GetGUID();

    if (player->InBattleground())
        return false;

    if (player->InBattlegroundQueue())
        return false;

    // if death revive
    /*if (player->isDead())
    {
        if (!GetEventValue(bot, "dead"))
        {
            uint32 randomTime =
                urand(sPlayerbotAIConfig->minRandomBotReviveTime, sPlayerbotAIConfig->maxRandomBotReviveTime);
            LOG_DEBUG("playerbots", "Mark bot {} as dead, will be revived in {}s.", player->GetName().c_str(),
                randomTime);
            SetEventValue(bot, "dead", 1, sPlayerbotAIConfig->maxRandomBotInWorldTime);
            SetEventValue(bot, "revive", 1, randomTime);
            return false;
        }

        if (!GetEventValue(bot, "revive"))
        {
            Revive(player);
            return true;
        }

        return false;
    }

    // leave group if leader is rndbot
    Group* group = player->GetGroup();
    if (group && !group->isLFGGroup() && IsRandomBot(group->GetLeader()))
    {
        player->RemoveFromGroup();
        LOG_INFO("playerbots", "Bot {} remove from group since leader is random bot.", player->GetName().c_str());
    }

    // only randomize and teleport idle bots
    bool idleBot = false;
    PlayerbotAI* botAI = GET_PLAYERBOT_AI(player);
    if (botAI)
    {
        if (TravelTarget* target = botAI->GetAiObjectContext()->GetValue<TravelTarget*>("travel target")->Get())
        {
            if (target->getTravelState() == TravelState::TRAVEL_STATE_IDLE)
            {
                idleBot = true;
            }
        }
        else
        {
            idleBot = true;
        }
    }
    if (idleBot)
    {
        // randomize
        uint32 randomize = GetEventValue(bot, "randomize");
        if (!randomize)
        {
            Randomize(player);
            LOG_DEBUG("playerbots", "Bot #{} {}:{} <{}>: randomized", bot,
                player->GetTeamId() == TEAM_ALLIANCE ? "A" : "H", player->GetLevel(), player->GetName());
            uint32 randomTime =
                urand(sPlayerbotAIConfig->minRandomBotRandomizeTime, sPlayerbotAIConfig->maxRandomBotRandomizeTime);
            ScheduleRandomize(bot, randomTime);
            return true;
        }

        uint32 teleport = GetEventValue(bot, "teleport");
        if (!teleport)
        {
            LOG_DEBUG("playerbots", "Bot #{} <{}>: teleport for level and refresh", bot, player->GetName());
            Refresh(player);
            RandomTeleportForLevel(player);
            uint32 time = urand(sPlayerbotAIConfig->minRandomBotTeleportInterval,
                sPlayerbotAIConfig->maxRandomBotTeleportInterval);
            ScheduleTeleport(bot, time);
            return true;
        }
    }

    return false;*/

    return true;
}

void RandomPlayerbotMgr::Revive(Player* player)
{
    uint64 bot = player->GetGUID();

    // LOG_INFO("playerbots", "Bot {} revived", player->GetName().c_str());
    //SetEventValue(bot, "dead", 0, 0);
    //SetEventValue(bot, "revive", 0, 0);

    Refresh(player);
    //RandomTeleportGrindForLevel(player);
}

void RandomPlayerbotMgr::Clear(Player* bot)
{
    //PlayerbotFactory factory(bot, bot->GetLevel());
    //factory.ClearEverything();
}

void RandomPlayerbotMgr::Refresh(Player* bot)
{
    PlayerbotAI* botAI = GET_PLAYERBOT_AI(bot);
    if (!botAI)
        return;

    if (bot->isDead())
    {
        bot->ResurrectPlayer(1.0f);
        bot->SpawnCorpseBones();
        //botAI->ResetStrategies(false);
    }

    // if (sPlayerbotAIConfig->disableRandomLevels)
    //     return;

    if (bot->InBattleground())
        return;

    SF_LOG_INFO("playerbots", "Refreshing bot #%u <%s>", bot->GetGUID(), bot->GetName().c_str());

    //botAI->Reset();
    bot->DurabilityRepairAll(false, 1.0f, false);
    bot->SetFullHealth();
    bot->SetPvP(true);
    //PlayerbotFactory factory(bot, bot->GetLevel());
    //factory.Refresh();

    if (bot->GetMaxPower(POWER_MANA) > 0)
        bot->SetPower(POWER_MANA, bot->GetMaxPower(POWER_MANA));

    if (bot->GetMaxPower(POWER_ENERGY) > 0)
        bot->SetPower(POWER_ENERGY, bot->GetMaxPower(POWER_ENERGY));

    //uint32 money = bot->GetMoney();
    //bot->SetMoney(money + 500 * sqrt(urand(1, bot->GetLevel() * 5)));

    if (bot->GetGroup())
        bot->RemoveFromGroup();
}

bool RandomPlayerbotMgr::IsRandomBot(Player* bot)
{
    if (bot && GET_PLAYERBOT_AI(bot))
    {
        if (GET_PLAYERBOT_AI(bot)->IsRealPlayer())
        {
            return false;
        }
    }
    if (bot)
    {
        return IsRandomBot(bot->GetGUID());
    }

    return false;
}

bool RandomPlayerbotMgr::IsRandomBot(uint64 bot)
{
    if (std::find(currentBots.begin(), currentBots.end(), bot) != currentBots.end())
    {
        return true;
    }

    return false;
}

void RandomPlayerbotMgr::GetBots()
{
    if (!currentBots.empty())
        return;

    /*PlayerbotsDatabasePreparedStatement* stmt =
        PlayerbotsDatabase.GetPreparedStatement(PLAYERBOTS_SEL_RANDOM_BOTS_BY_OWNER_AND_EVENT);
    stmt->SetData(0, 0);
    stmt->SetData(1, "add");
    uint32 maxAllowedBotCount = GetEventValue(0, "bot_count");
    if (PreparedQueryResult result = PlayerbotsDatabase.Query(stmt))
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 bot = fields[0].Get<uint32>();
            if (GetEventValue(bot, "add"))
                currentBots.push_back(bot);

            if (currentBots.size() >= maxAllowedBotCount)
                break;
        } while (result->NextRow());
    }*/
}

uint32 RandomPlayerbotMgr::GetEventValue(uint64 bot, std::string const event)
{
    /*
    // load all events at once on first event load
    if (eventCache[bot].empty())
    {
        PlayerbotsDatabasePreparedStatement* stmt =
            PlayerbotsDatabase.GetPreparedStatement(PLAYERBOTS_SEL_RANDOM_BOTS_BY_OWNER_AND_BOT);
        stmt->SetData(0, 0);
        stmt->SetData(1, bot);
        if (PreparedQueryResult result = PlayerbotsDatabase.Query(stmt))
        {
            do
            {
                Field* fields = result->Fetch();
                std::string const eventName = fields[0].Get<std::string>();

                CachedEvent e;
                e.value = fields[1].Get<uint32>();
                e.lastChangeTime = fields[2].Get<uint32>();
                e.validIn = fields[3].Get<uint32>();
                e.data = fields[4].Get<std::string>();
                eventCache[bot][eventName] = std::move(e);
            } while (result->NextRow());
        }
    }

    CachedEvent& e = eventCache[bot][event];
    if ((time(0) - e.lastChangeTime) >= e.validIn && event != "specNo" && event != "specLink")
        e.value = 0;

    return e.value;*/

    return 0;
}

std::string const RandomPlayerbotMgr::GetEventData(uint64 bot, std::string const event)
{
    std::string data = "";
    /*if (GetEventValue(bot, event))
    {
        CachedEvent e = eventCache[bot][event];
        data = e.data;
    }*/

    return data;
}

uint32 RandomPlayerbotMgr::SetEventValue(uint64 bot, std::string const event, uint32 value, uint32 validIn,
    std::string const data)
{
    /*PlayerbotsDatabaseTransaction trans = PlayerbotsDatabase.BeginTransaction();

    PlayerbotsDatabasePreparedStatement* stmt =
        PlayerbotsDatabase.GetPreparedStatement(PLAYERBOTS_DEL_RANDOM_BOTS_BY_OWNER_AND_EVENT);
    stmt->SetData(0, 0);
    stmt->SetData(1, bot);
    stmt->SetData(2, event.c_str());
    trans->Append(stmt);

    if (value)
    {
        stmt = PlayerbotsDatabase.GetPreparedStatement(PLAYERBOTS_INS_RANDOM_BOTS);
        stmt->SetData(0, 0);
        stmt->SetData(1, bot);
        stmt->SetData(2, static_cast<uint32>(GameTime::GetGameTime().count()));
        stmt->SetData(3, validIn);
        stmt->SetData(4, event.c_str());
        stmt->SetData(5, value);
        if (data != "")
        {
            stmt->SetData(6, data.c_str());
        }
        else
        {
            stmt->SetData(6);
        }
        trans->Append(stmt);
    }

    PlayerbotsDatabase.CommitTransaction(trans);

    CachedEvent e(value, (uint32)time(nullptr), validIn, data);
    eventCache[bot][event] = std::move(e);*/
    return value;
}

uint32 RandomPlayerbotMgr::GetValue(uint64 bot, std::string const type) { return GetEventValue(bot, type); }

uint32 RandomPlayerbotMgr::GetValue(Player* bot, std::string const type)
{
    return GetValue(bot->GetGUID(), type);
}

std::string const RandomPlayerbotMgr::GetData(uint32 bot, std::string const type) { return GetEventData(bot, type); }

void RandomPlayerbotMgr::SetValue(uint64 bot, std::string const type, uint32 value, std::string const data)
{
    SetEventValue(bot, type, value, 5/*sPlayerbotAIConfig->maxRandomBotInWorldTime*/, data);
}

void RandomPlayerbotMgr::SetValue(Player* bot, std::string const type, uint32 value, std::string const data)
{
    SetValue(bot->GetGUID(), type, value, data);
}

void RandomPlayerbotMgr::OnPlayerLogout(Player* player)
{
    DisablePlayerBot(player->GetGUID());

    for (PlayerBotMap::const_iterator it = GetPlayerBotsBegin(); it != GetPlayerBotsEnd(); ++it)
    {
        Player* const bot = it->second;
        PlayerbotAI* botAI = GET_PLAYERBOT_AI(bot);
        if (botAI && player == botAI->GetMaster())
        {
            botAI->SetMaster(nullptr);
            if (!bot->InBattleground())
            {
                //botAI->ResetStrategies();
            }
        }
    }

    std::vector<Player*>::iterator i = std::find(players.begin(), players.end(), player);
    if (i != players.end())
        players.erase(i);
}

void RandomPlayerbotMgr::OnBotLoginInternal(Player* const bot)
{
    SF_LOG_INFO("playerbots", "%u/%u Bot %s logged in", playerBots.size(), sRandomPlayerbotMgr->GetMaxAllowedBotCount(),
        bot->GetName().c_str());

    /*if (sPlayerbotAIConfig->randomBotFixedLevel)
    {
        bot->SetPlayerFlag(PLAYER_FLAGS_NO_XP_GAIN);
    }
    else
    {
        bot->RemovePlayerFlag(PLAYER_FLAGS_NO_XP_GAIN);
    }*/
}

void RandomPlayerbotMgr::OnPlayerLogin(Player* player)
{
    uint32 botsNearby = 0;

    for (PlayerBotMap::const_iterator it = GetPlayerBotsBegin(); it != GetPlayerBotsEnd(); ++it)
    {
        Player* const bot = it->second;
        if (player == bot /* || GET_PLAYERBOT_AI(player)*/)  // TEST
            continue;

        Cell playerCell(player->GetPositionX(), player->GetPositionY());
        Cell botCell(bot->GetPositionX(), bot->GetPositionY());

        // if (playerCell == botCell)
        // botsNearby++;

        Group* group = bot->GetGroup();
        if (!group)
            continue;

        for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
        {
            Player* member = gref->GetSource();
            PlayerbotAI* botAI = GET_PLAYERBOT_AI(bot);
            if (botAI && member == player && (!botAI->GetMaster() || GET_PLAYERBOT_AI(botAI->GetMaster())))
            {
                if (!bot->InBattleground())
                {
                    botAI->SetMaster(player);
                    //botAI->ResetStrategies();
                    //botAI->TellMaster("Hello");
                }

                break;
            }
        }
    }

    if (botsNearby > 100 && false)
    {
        //WorldPosition botPos(player);

        // botPos.GetReachableRandomPointOnGround(player, sPlayerbotAIConfig->reactDistance * 2, true);

        // player->TeleportTo(botPos);
        // player->Relocate(botPos.coord_x, botPos.coord_y, botPos.coord_z, botPos.orientation);

        /*if (!player->GetFactionTemplateEntry())
        {
            botPos.GetReachableRandomPointOnGround(player, sPlayerbotAIConfig->reactDistance * 2, true);
        }
        else
        {
            std::vector<TravelDestination*> dests = sTravelMgr->getRpgTravelDestinations(player, true, true, 200000.0f);

            do
            {
                RpgTravelDestination* dest = (RpgTravelDestination*)dests[urand(0, dests.size() - 1)];
                CreatureTemplate const* cInfo = dest->GetCreatureTemplate();
                if (!cInfo)
                    continue;

                FactionTemplateEntry const* factionEntry = sFactionTemplateStore.LookupEntry(cInfo->faction);
                ReputationRank reaction = Unit::GetFactionReactionTo(player->GetFactionTemplateEntry(), factionEntry);

                if (reaction > REP_NEUTRAL && dest->nearestPoint(&botPos)->m_mapId == player->GetMapId())
                {
                    botPos = *dest->nearestPoint(&botPos);
                    break;
                }
            } while (true);
        }

        player->TeleportTo(botPos);*/
    }

    /* TODO REMOVE THIS ITS ONLY TO GET THE PACET WORKING ATM*/
    currentBots.push_back(player->GetGUID());

    if (!IsRandomBot(player))
    {
        players.push_back(player);
        SF_LOG_INFO("playerbots", "Including non-random bot player %s into random bot update", player->GetName().c_str());
    }
}

void RandomPlayerbotMgr::OnPlayerLoginError(uint64 bot)
{
    SetEventValue(bot, "add", 0, 0);
    currentBots.erase(std::remove(currentBots.begin(), currentBots.end(), bot), currentBots.end());
}

Player* RandomPlayerbotMgr::GetRandomPlayer()
{
    if (players.empty())
        return nullptr;

    uint32 index = std::rand() % players.size();
    return players[index];
}
