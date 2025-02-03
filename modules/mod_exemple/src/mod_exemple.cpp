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

class exemple_announce : public PlayerScript
{
public:
    exemple_announce() : PlayerScript("exemple_announce") {}

    void OnLogin(Player* player, bool /*firstlogin*/) override
    {
        // Announce Module
        ChatHandler(player->GetSession()).SendSysMessage("This server is running the |cff4CFF00exemple announce |rmodule.");
    }

};


extern void AddSC_mod_exemple()
{
    new exemple_announce();
}
