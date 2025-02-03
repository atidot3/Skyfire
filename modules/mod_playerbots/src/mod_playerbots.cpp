/*
** Made by Traesh https://github.com/Traesh
** AzerothCore 2019 http://www.azerothcore.org/
** Conan513 https://github.com/conan513
** Made into a module by Micrah https://github.com/milestorme/
*/

#include "Player.h"
#include "ScriptMgr.h"
#include "Config.h"
#include "World.h"
#include "Chat.h"
#include "WorldPacket.h"
#include "Log.h"
#include "CharacterHandler.h"

#include "Authentication/AuthCrypt.h"
#include "WorldSocket.h"

#include "Playerbots.h"
#include "RandomPlayerbotMgr.h"

#include <unordered_set>

static const char* QUERY_ACCOUNT = "SELECT * FROM `account`";
static const char* QUERY_CHARACTER = "SELECT * FROM `characters`";

class mod_playerbots : public PlayerScript
{
public:
    mod_playerbots() : PlayerScript("mod_playerbots") {}

    void OnLogin(Player* player, bool /*firstlogin*/) override
    {
        // Announce Module
        ChatHandler(player->GetSession()).SendSysMessage("This server is running the |cff4CFF00mod playerbots |rmodule.");
    }

};

class PlayerbotsWorldScript : public WorldScript
{
public:
    PlayerbotsWorldScript() : WorldScript("PlayerbotsWorldScript") {}

    void OnConfigLoad(bool reloaded) override
    {
        if (!reloaded)
        {
            uint32 oldMSTime = getMSTime();

            SF_LOG_INFO("playerbots", " ");
            SF_LOG_INFO("playerbots", "Load Playerbots Config...");

            

            SF_LOG_INFO("playerbots", ">> Loaded playerbots config in %u ms", GetMSTimeDiffToNow(oldMSTime));
            SF_LOG_INFO("playerbots", " ");
        }
    }

    void LogPlayer()
    {
        std::list<uint32> characters_entry;
        QueryResult result = LoginDatabase.PQuery(QUERY_ACCOUNT, "");
        if (!result.get())
        {
            SF_LOG_INFO("playerbots", "Fetch database auth failed on get bots account");
            return;
        }
        do
        {
            Field* fields = result.get()->Fetch();

            auto account = fields[0].GetUInt32();
            auto username = fields[1].GetString();

            SF_LOG_INFO("playerbots", "Account found id: %u [%s]", account, username);

            if (username.contains("RNDBOT"))
            {
                SF_LOG_INFO("playerbots", "Bot account found id: %u", account);
                QueryResult result = CharacterDatabase.PQuery(QUERY_CHARACTER, account);
                if (!result.get())
                {
                    SF_LOG_INFO("playerbots", "Fetch database characters failed on get bots characters");
                    return;
                }
                do
                {
                    Field* field = result.get()->Fetch();

                    auto guid = field[0].GetUInt32();
                    auto accountid = field[2].GetUInt32();

                    SF_LOG_INFO("playerbots", "characters found guid: %u accountid: %u", guid, accountid);

                    if (accountid == account)
                    {
                        characters_entry.push_back(accountid);
                        sRandomPlayerbotMgr->AddPlayerBot(guid, 0);
                    }
                } while (result.get()->NextRow());

            }
        } while (result.get()->NextRow());
        SF_LOG_INFO("playerbots", "Found bot characters: %u", characters_entry.size());
    }

    void OnStartup() override
    {
        SF_LOG_INFO("playerbots", "Playerbots OnStartup...");
        LogPlayer();
    }
};

class PlayerbotsServerScript : public ServerScript
{
public:
    PlayerbotsServerScript() : ServerScript("PlayerbotsServerScript") {}
    virtual void OnPacketReceive(WorldSocket* socket, WorldPacket& packet) override
    {
        WorldSession* sessionBot = socket->GetSession();
        if (sessionBot)
        {
            Player* playerBot = sessionBot->GetPlayer();
            if (playerBot)
            {
                SF_LOG_INFO("playerbots", "Player: %s Received packet", playerBot->GetName().c_str());
            }
            if (PlayerbotMgr* playerbotMgr = GET_PLAYERBOT_MGR(playerBot))
                playerbotMgr->HandleMasterIncomingPacket(packet);
        }
    }
};

class PlayerbotsScript : public PlayerbotScript
{
public:
    PlayerbotsScript() : PlayerbotScript("PlayerbotsScript") {}

    void OnPlayerbotCheckKillTask(Player* /*player*/, Unit* /*victim*/) override
    {
    }

    void OnPlayerbotCheckPetitionAccount(Player* player, bool& found) override
    {
        if (found && GET_PLAYERBOT_AI(player))
            found = false;
    }

    bool OnPlayerbotCheckUpdatesToSend(Player* player) override
    {
        if (PlayerbotAI* botAI = GET_PLAYERBOT_AI(player))
            return botAI->IsRealPlayer();

        return true;
    }

    void OnPlayerbotPacketSent(Player* player, WorldPacket const* packet) override
    {
        if (!player || !sRandomPlayerbotMgr->IsRandomBot(player))
            return;
        
        SF_LOG_INFO("playerbots", "Player: %s Received packet %s", player->GetName().c_str(), GetOpcodeNameForLogging(packet->GetOpcode(), true).c_str());

        if (packet->GetOpcode() == SMSG_TIME_SYNC_REQUEST)
        {
            WorldPacket p = *packet;
            uint32 counter;
            p >> counter;
            uint32 clientTicks = time(NULL);
            WorldPacket packet(CMSG_TIME_SYNC_RESPONSE);
            packet.rpos(0);
            packet << counter << clientTicks;

            player->GetSession()->HandleTimeSyncResp(packet);
        }
        if (PlayerbotAI* botAI = GET_PLAYERBOT_AI(player))
        {
            botAI->HandleBotOutgoingPacket(*packet);
        }
        if (PlayerbotMgr* playerbotMgr = GET_PLAYERBOT_MGR(player))
        {
            playerbotMgr->HandleMasterOutgoingPacket(*packet);
        }
    }

    void OnPlayerbotUpdate(uint32 diff) override
    {
        sRandomPlayerbotMgr->UpdateAI(diff);
        sRandomPlayerbotMgr->UpdateSessions();
    }

    void OnPlayerbotUpdateSessions(Player* player) override
    {
        if (player)
            if (PlayerbotMgr* playerbotMgr = GET_PLAYERBOT_MGR(player))
                playerbotMgr->UpdateSessions();
    }

    void OnPlayerbotLogout(Player* player) override
    {
        if (PlayerbotMgr* playerbotMgr = GET_PLAYERBOT_MGR(player))
        {
            PlayerbotAI* botAI = GET_PLAYERBOT_AI(player);
            if (!botAI || botAI->IsRealPlayer())
            {
                playerbotMgr->LogoutAllBots();
            }
        }

        sRandomPlayerbotMgr->OnPlayerLogout(player);
    }

    void OnPlayerbotLogoutBots() override { sRandomPlayerbotMgr->LogoutAllBots(); }
};

void AddSC_mod_playerbots()
{
    new mod_playerbots();

    new PlayerbotsWorldScript();
    new PlayerbotsScript();
}
