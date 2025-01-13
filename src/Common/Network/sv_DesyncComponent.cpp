#include "Common/Network/sv_DesyncComponent.h"

#include <entt/entity/registry.hpp>
#include <entt/entity/handle.hpp>

namespace ACGame
{
    void DesyncComponent::AddComponent(entt::handle handle)
    {
        AddComponent(*handle.registry(), handle.entity());
    }

    void DesyncComponent::AddComponent(entt::registry& registry, entt::entity entity)
    {
        if(!registry.any_of<DesyncComponent>(entity))
            registry.emplace<DesyncComponent>(entity);
    }
}
