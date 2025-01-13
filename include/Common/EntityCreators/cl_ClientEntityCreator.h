#pragma once
#include <string>
#include <entt/entity/registry.hpp>

#include "Common/Character/sh_CharacterAppearance.h"
#include "Common/Grid/sh_GridData.h"
#include "Neela/Graphics/Transform.h"

namespace ACGame
{
    class ClientEntityCreator
    {
    public:
        // Character
        static entt::handle CreateClientCharacterEntity(entt::registry& registry,
                                                 const std::string& name,
                                                 std::shared_ptr<Neela::Transform> transform = std::make_shared<Neela::Transform>());

        static entt::handle CreateClientAppearancePartEntity(entt::registry& registry,
                                                      const std::string& name,
                                                      AppearancePart partType,
                                                      std::shared_ptr<Neela::Transform> transform);

        // Player Character
        static entt::handle CreateClientPlayerCharacterEntity(entt::registry& registry, const std::string& name,
                                                       std::shared_ptr<Neela::Transform> transform = std::make_shared<Neela::Transform>());

        // Grid
        static entt::handle CreateClientGridEntity(entt::registry& registry, const std::string& name, Neela::Vector2f position,
                                            const GridData* gridData);
    };
}
