/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#ifndef _PLAYERBOT_PLAYERBOTAI_H
#define _PLAYERBOT_PLAYERBOTAI_H

#include <queue>
#include <stack>

#include "PlayerbotAIBase.h"
#include "WorldPacket.h"

enum BotState
{
    BOT_STATE_COMBAT = 0,
    BOT_STATE_NON_COMBAT = 1,
    BOT_STATE_DEAD = 2,

    BOT_STATE_MAX
};

class Position;
class Player;
class PlayerbotAI : public PlayerbotAIBase
{
public:
    PlayerbotAI();
    PlayerbotAI(Player* bot);
    virtual ~PlayerbotAI();

    void UpdateAI(uint32 elapsed, bool minimal = false) override;
    void UpdateAIInternal(uint32 elapsed, bool minimal = false) override;

    void HandleBotOutgoingPacket(WorldPacket const* packet);
    void HandleMasterIncomingPacket(WorldPacket const* packet);
    void HandleMasterOutgoingPacket(WorldPacket const* packet);
    void HandleTeleportAck();
    

    Player* GetBot() { return bot; }
    Player* GetMaster() { return master; }

    // Checks if the bot is really a player. Players always have themselves as master.
    bool IsRealPlayer() { return master ? (master == bot) : false; }
    // Bot has a master that is a player.
    bool HasRealPlayerMaster();
    // Bot has a master that is activly playing.
    bool HasActivePlayerMaster();
    // Get the group leader or the master of the bot.
    // Checks if the bot is summoned as alt of a player
    bool IsAlt();

    void SetMaster(Player* newMaster) { master = newMaster; }

    bool CanMove();
private:
    bool _isBotInitializing = false;

protected:
    Player* bot;
    Player* master;
    uint32 accountId;
    //AiObjectContext* aiObjectContext;
    //Engine* currentEngine;
    //Engine* engines[BOT_STATE_MAX];
    BotState currentState;
    //ChatHelper chatHelper;
    //std::list<ChatCommandHolder> chatCommands;
    //std::list<ChatQueuedReply> chatReplies;
    //PacketHandlingHelper botOutgoingPacketHandlers;
    //PacketHandlingHelper masterIncomingPacketHandlers;
    //PacketHandlingHelper masterOutgoingPacketHandlers;
    //CompositeChatFilter chatFilter;
    //PlayerbotSecurity security;
    //std::map<std::string, time_t> whispers;
    //std::pair<ChatMsg, time_t> currentChat;
    //static std::set<std::string> unsecuredCommands;
    //bool allowActive[MAX_ACTIVITY_TYPE];
    //time_t allowActiveCheckTimer[MAX_ACTIVITY_TYPE];
    //bool inCombat = false;
    //BotCheatMask cheatMask = BotCheatMask::none;
    //Position jumpDestination = Position();
    //uint32 nextTransportCheck = 0;
};

#endif