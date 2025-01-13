#pragma once
#include <entt/fwd.hpp>

namespace ACGame
{
    /**
     * @brief Component attached to Networked entities when they want to be included in a packet sending all updates.
     * All DesyncComponents are getted regularly to send changes to the clients
     */
    struct DesyncComponent
    {
        /**
         * @brief Add if needed a DesyncComponent to an entity
         * @param handle Target entity
         */
        static void AddComponent(entt::handle handle);
        static void AddComponent(entt::registry& registry, entt::entity entity);
    };
}
