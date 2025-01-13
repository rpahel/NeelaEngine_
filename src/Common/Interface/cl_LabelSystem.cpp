#include "Common/Interface/cl_LabelSystem.h"

#include "Common/EntityCreators/cl_ClientEntityCreator.h"
#include "Common/Interface/cl_LabelComponent.h"
#include "Neela/Components/TransformComponent.h"
#include "Neela/Essentials/EntityCreator.h"

namespace ACGame
{
    LabelSystem::LabelSystem(entt::registry* registry) :
    m_registry(registry),
    m_cameraTransform(nullptr),
    m_labelObserver( *registry, entt::collector.group<LabelComponent, Neela::TransformComponent>())
    {
        registry->on_destroy<LabelComponent>().connect<&LabelSystem::OnLabelComponentDestruct>(this);
    }

    void LabelSystem::Apply()
    {
        m_labelObserver.each([this](entt::entity entity)
        {
            m_spawnedInterfaceEntities.emplace(entity,
                Neela::EntityCreator::CreateInterfaceEntity(*m_registry, "Label Interface " + std::to_string(static_cast<std::uint32_t>(entity)),
                    [this, entity]()
                    {
                        if(m_cameraTransform == nullptr)
                            return false;
                        if(!m_registry->valid(entity))
                            return true;

                        const LabelComponent* labelComponent = m_registry->try_get<LabelComponent>(entity);
                        if(!labelComponent)
                            return true;
                        
                        ImGuiWindowFlags flags = 0;
                        flags |= ImGuiWindowFlags_NoTitleBar;
                        flags |= ImGuiWindowFlags_AlwaysAutoResize;
                        flags |= ImGuiWindowFlags_NoMove;

                        Neela::Vector2f windowPos = m_cameraTransform->AbsoluteToRelativePosition(m_registry->get<Neela::TransformComponent>(entity).m_Transform->GetAbsolutePosition());
                        windowPos.X += labelComponent->offset.X;
                        windowPos.Y += labelComponent->offset.Y;
                        windowPos -= ImGui::CalcTextSize(labelComponent->label.c_str()).x * 0.5f + 10;

                        ImGui::SetNextWindowBgAlpha(0.5f);
                        ImGui::SetNextWindowPos(ImVec2(windowPos.X, windowPos.Y));
                        std::string windowName = "Label " + std::to_string(static_cast<std::uint32_t>(entity));
                        if(ImGui::Begin(windowName.c_str(), nullptr, flags))
                        {
                            ImGui::Text(labelComponent->label.c_str());
                        }
                        ImGui::End();
                        
                        return false;
                    }).entity());
        });
    }

    void LabelSystem::SetCameraTransform(const std::shared_ptr<Neela::Transform>& cameraTransform)
    {
        m_cameraTransform = cameraTransform;
    }

    void LabelSystem::OnLabelComponentDestruct(entt::registry& registry, entt::entity entity)
    {
        if(!m_spawnedInterfaceEntities.contains(entity))
            return;
        m_registry->destroy(m_spawnedInterfaceEntities[entity]);
    }
}
