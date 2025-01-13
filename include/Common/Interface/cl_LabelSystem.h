#pragma once
#include <entt/entity/registry.hpp>
#include <entt/entity/observer.hpp>
#include <entt/entity/entity.hpp>

#include "Neela/Graphics/Transform.h"

namespace ACGame
{
    /**
     * @brief System spawning/destroying interface entities for each created LabelComponent in a registry
     */
    class LabelSystem
    {
    private:
        entt::registry* m_registry;
        std::shared_ptr<Neela::Transform> m_cameraTransform;

        entt::observer m_labelObserver;
        std::unordered_map<entt::entity, entt::entity> m_spawnedInterfaceEntities;
        
    public:
        LabelSystem(entt::registry* registry);

        void Apply();

        void SetCameraTransform(const std::shared_ptr<Neela::Transform>& cameraTransform);

    private:
        void OnLabelComponentDestruct(entt::registry& registry, entt::entity entity);
    };
}
