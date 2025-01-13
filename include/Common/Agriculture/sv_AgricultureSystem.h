#pragma once
#include <entt/entity/registry.hpp>

#include "Common/sh_GameData.h"
#include "Common/Network/sv_MessageHandler.h"

namespace ACGame
{
    class AgricultureSystem
    {
    public:
        static void Apply(entt::registry& registry, ACNet::SV_MessageHandler& messageHandler, GameData* gameData, float deltaTime);
    };
}
