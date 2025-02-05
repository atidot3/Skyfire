/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "AiFactory.h"

#include "BattlegroundMgr.h"

#include "Engine.h"
#include "Group.h"

#include "Item.h"

#include "PlayerbotAI.h"
#include "PlayerbotAIConfig.h"
#include "Playerbots.h"

#include "SharedDefines.h"
#include "SpellInfo.h"
#include "SpellMgr.h"


AiObjectContext* AiFactory::createAiObjectContext(Player* player, PlayerbotAI* botAI)
{
    /*switch (player->getClass())
    {
        case CLASS_PRIEST:
            return new PriestAiObjectContext(botAI);
        case CLASS_MAGE:
            return new MageAiObjectContext(botAI);
        case CLASS_WARLOCK:
            return new WarlockAiObjectContext(botAI);
        case CLASS_WARRIOR:
            return new WarriorAiObjectContext(botAI);
        case CLASS_SHAMAN:
            return new ShamanAiObjectContext(botAI);
        case CLASS_PALADIN:
            return new PaladinAiObjectContext(botAI);
        case CLASS_DRUID:
            return new DruidAiObjectContext(botAI);
        case CLASS_HUNTER:
            return new HunterAiObjectContext(botAI);
        case CLASS_ROGUE:
            return new RogueAiObjectContext(botAI);
        case CLASS_DEATH_KNIGHT:
            return new DKAiObjectContext(botAI);
    }*/

    return new AiObjectContext(botAI);
}

uint8 AiFactory::GetPlayerSpecTab(Player* bot)
{
    return 0;
}

std::map<uint8, uint32> AiFactory::GetPlayerSpecTabs(Player* bot)
{
    std::map<uint8, uint32> tabs = {{0, 0}, {0, 0}, {0, 0}};
    /*const PlayerTalentMap& talentMap = bot->GetTalentMap();
    for (PlayerTalentMap::const_iterator i = talentMap.begin(); i != talentMap.end(); ++i)
    {
        uint32 spellId = i->first;
        if ((bot->GetActiveSpecMask() & i->second->specMask) == 0)
        {
            continue;
        }
        TalentSpellPos const* talentPos = GetTalentSpellPos(spellId);
        if (!talentPos)
            continue;
        TalentEntry const* talentInfo = sTalentStore.LookupEntry(talentPos->talent_id);
        if (!talentInfo)
            continue;

        uint32 const* talentTabIds = GetTalentTabPages(bot->getClass());

        const SpellInfo* spellInfo = sSpellMgr->GetSpellInfo(spellId);
        int rank = spellInfo ? spellInfo->GetRank() : 1;
        if (talentInfo->TalentTab == talentTabIds[0])
            tabs[0] += rank;
        if (talentInfo->TalentTab == talentTabIds[1])
            tabs[1] += rank;
        if (talentInfo->TalentTab == talentTabIds[2])
            tabs[2] += rank;
    }
    return tabs;*/
}

BotRoles AiFactory::GetPlayerRoles(Player* player)
{
    BotRoles role = BOT_ROLE_NONE;
    uint8 tab = GetPlayerSpecTab(player);

    switch (player->getClass())
    {
        case CLASS_PRIEST:
            if (tab == 2)
                role = BOT_ROLE_DPS;
            else
                role = BOT_ROLE_HEALER;
            break;
        case CLASS_SHAMAN:
            if (tab == 2)
                role = BOT_ROLE_HEALER;
            else
                role = BOT_ROLE_DPS;
            break;
        case CLASS_WARRIOR:
            if (tab == 2)
                role = BOT_ROLE_TANK;
            else
                role = BOT_ROLE_DPS;
            break;
        case CLASS_PALADIN:
            if (tab == 0)
                role = BOT_ROLE_HEALER;
            else if (tab == 1)
                role = BOT_ROLE_TANK;
            else if (tab == 2)
                role = BOT_ROLE_DPS;
            break;
        case CLASS_DRUID:
            if (tab == 0)
                role = BOT_ROLE_DPS;
            else if (tab == 1)
                role = (BotRoles)(BOT_ROLE_TANK | BOT_ROLE_DPS);
            else if (tab == 2)
                role = BOT_ROLE_HEALER;
            break;
        default:
            role = BOT_ROLE_DPS;
            break;
    }

    return role;
}

std::string AiFactory::GetPlayerSpecName(Player* player)
{
    std::string specName;
    int tab = GetPlayerSpecTab(player);
    switch (player->getClass())
    {
        case CLASS_PRIEST:
            if (tab == 2)
                specName = "shadow";
            else if (tab == 1)
                specName = "holy";
            else
                specName = "disc";
            ;
            break;
        case CLASS_SHAMAN:
            if (tab == 2)
                specName = "resto";
            else if (tab == 1)
                specName = "enhance";
            else
                specName = "elem";
            break;
        case CLASS_WARRIOR:
            if (tab == 2)
                specName = "prot";
            else if (tab == 1)
                specName = "fury";
            else
                specName = "arms";
            break;
        case CLASS_PALADIN:
            if (tab == 0)
                specName = "holy";
            else if (tab == 1)
                specName = "prot";
            else if (tab == 2)
                specName = "retrib";
            break;
        case CLASS_DRUID:
            if (tab == 0)
                specName = "balance";
            else if (tab == 1)
                specName = "feraldps";
            else if (tab == 2)
                specName = "resto";
            break;
        case CLASS_ROGUE:
            if (tab == 0)
                specName = "assas";
            else if (tab == 1)
                specName = "combat";
            else if (tab == 2)
                specName = "subtle";
            break;
        case CLASS_HUNTER:
            if (tab == 0)
                specName = "beast";
            else if (tab == 1)
                specName = "marks";
            else if (tab == 2)
                specName = "surv";
            break;
        case CLASS_DEATH_KNIGHT:
            if (tab == 0)
                specName = "blooddps";
            else if (tab == 1)
                specName = "frostdps";
            else if (tab == 2)
                specName = "unholydps";
            break;
        case CLASS_MAGE:
            if (tab == 0)
                specName = "arcane";
            else if (tab == 1)
                specName = "fire";
            else if (tab == 2)
                specName = "frost";
            break;
        case CLASS_WARLOCK:
            if (tab == 0)
                specName = "afflic";
            else if (tab == 1)
                specName = "demo";
            else if (tab == 2)
                specName = "destro";
            break;
        default:
            break;
    }

    return specName;
}

void AiFactory::AddDefaultCombatStrategies(Player* player, PlayerbotAI* const facade, Engine* engine)
{
    if (!player->InBattleground())
    {
        engine->addStrategiesNoInit("racials", "chat", "default", "cast time", "duel", "boost", nullptr);
    }
    if (sPlayerbotAIConfig->autoAvoidAoe && facade->HasRealPlayerMaster())
    {
        engine->addStrategy("avoid aoe", false);
    }
    engine->addStrategy("formation", false);
}

Engine* AiFactory::createCombatEngine(Player* player, PlayerbotAI* const facade, AiObjectContext* aiObjectContext)
{
    Engine* engine = new Engine(facade, aiObjectContext);
    AddDefaultCombatStrategies(player, facade, engine);
    engine->Init();
    return engine;
}

void AiFactory::AddDefaultNonCombatStrategies(Player* player, PlayerbotAI* const facade, Engine* nonCombatEngine)
{
}

Engine* AiFactory::createNonCombatEngine(Player* player, PlayerbotAI* const facade, AiObjectContext* aiObjectContext)
{
    Engine* nonCombatEngine = new Engine(facade, aiObjectContext);

    AddDefaultNonCombatStrategies(player, facade, nonCombatEngine);
    nonCombatEngine->Init();
    return nonCombatEngine;
}

void AiFactory::AddDefaultDeadStrategies(Player* player, PlayerbotAI* const facade, Engine* deadEngine)
{
    (void)facade;  // unused and remove warning
    deadEngine->addStrategiesNoInit("dead", "stay", "chat", "default", "follow", nullptr);

    if (sRandomPlayerbotMgr->IsRandomBot(player) && !player->GetGroup())
    {
        deadEngine->removeStrategy("follow", false);
    }
}

Engine* AiFactory::createDeadEngine(Player* player, PlayerbotAI* const facade, AiObjectContext* AiObjectContext)
{
    Engine* deadEngine = new Engine(facade, AiObjectContext);
    AddDefaultDeadStrategies(player, facade, deadEngine);
    deadEngine->Init();
    return deadEngine;
}
