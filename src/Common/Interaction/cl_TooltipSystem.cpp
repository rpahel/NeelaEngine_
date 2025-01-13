#include "Common/Interaction/cl_TooltipSystem.h"

#include "Common/sh_logger.h"
#include "Common/Interaction/cl_InteractionTooltipComponent.h"
#include "Neela/Essentials/EntityCreator.h"

namespace ACGame
{
    TooltipSystem::TooltipSystem(entt::registry* registry) :
    m_registry(registry),
    m_interfaceEntity(entt::null)
    {
        assert(m_registry);
    }

    void TooltipSystem::Apply(const Neela::Vector2f& screenMousePos, entt::entity hoveredEntity)
    {
        if (!m_registry->valid(hoveredEntity))
        {
            m_tooltipMessage.clear();
            return;
        }
        
        if(!m_registry->valid(m_interfaceEntity))
            CreateInterfaceHandle();


        if (InteractionTooltipComponent* tooltipComponent = m_registry->try_get<InteractionTooltipComponent>(hoveredEntity))
        {
            m_windowPosition = ImVec2(screenMousePos.X, screenMousePos.Y);
            m_tooltipMessage = tooltipComponent->tooltip;
        }
        else
        {
            m_tooltipMessage.clear();
        }
    }

    void TooltipSystem::CreateInterfaceHandle()
    {
        if(m_registry->valid(m_interfaceEntity))
            m_registry->destroy(m_interfaceEntity);
        
        m_interfaceEntity = Neela::EntityCreator::CreateInterfaceEntity(*m_registry, "TooltipInterface",
            [this]()
            {
                if(m_tooltipMessage.empty())
                    return false;
                
                ImGuiWindowFlags flags = 0;
                flags |= ImGuiWindowFlags_NoTitleBar;
                // flags |= ImGuiWindowFlags_NoBackground;
                flags |= ImGuiChildFlags_AlwaysAutoResize;

                ImGui::SetNextWindowPos(m_windowPosition);
                if(ImGui::Begin("Tooltip", nullptr, flags))
                {
                    ImGui::Text(m_tooltipMessage.c_str());
                }
                ImGui::End();
                
                return false;
            }).entity();
    }
}
