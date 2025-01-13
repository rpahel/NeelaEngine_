#pragma once
#include <imgui.h>
#include <entt/entity/registry.hpp>
#include <entt/entity/handle.hpp>

#include "Neela/Maths/Vector.h"

namespace ACGame
{
    class TooltipSystem
    {
    private:
        entt::registry* m_registry;
        entt::entity m_interfaceEntity;
        ImVec2 m_windowPosition;

        std::string m_tooltipMessage;
        
    public:
        explicit TooltipSystem(entt::registry* registry);

        void Apply(const Neela::Vector2f& screenMousePos, entt::entity hoveredEntity);
    private:
        void CreateInterfaceHandle();
    };
}
