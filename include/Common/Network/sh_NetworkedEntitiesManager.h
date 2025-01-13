#pragma once
#include <entt/entt.hpp>

#include "Common/Network/sv_MessageHandler.h"

namespace ACNet
{
    /**
     * @brief Class responsible for registering entities with a NetworkComponentId
     */
    class NetworkedEntitiesManager
    {
    protected:
        entt::registry* m_registry;
        std::unordered_map<NetworkEntityId, entt::entity> m_networkedEntities;
    
    public:
        //==== Tick ====
        virtual void Tick() = 0;

        //==== Entities getters ====
        bool HasNetworkedEntity(NetworkEntityId networkId);
        entt::handle GetNetworkedEntity(NetworkEntityId networkId);
        
        template<class T>
        T& GetComponent(NetworkEntityId networkId);

        template<class T>
        T* TryGetComponent(NetworkEntityId networkId);

    protected:
        //==== Constructors ====
        NetworkedEntitiesManager(entt::registry* registry);
        
        virtual void OnNetworkedConstruct(entt::handle handle, NetworkComponent& networkComponent) = 0;
        virtual void OnNetworkedDestruct(entt::handle handle, NetworkComponent& networkComponent) = 0;

    private:
        void InternalOnNetworkedConstruct(entt::registry& registry, entt::entity entity);
        void InternalOnNetworkedDestruct(entt::registry& registry, entt::entity entity);
    };

    template <class T>
    T& NetworkedEntitiesManager::GetComponent(NetworkEntityId networkId)
    {
        if (!HasNetworkedEntity(networkId))
            throw std::runtime_error("Cannot get component of a non existing networked entity");
        return m_registry->get<T>(m_networkedEntities[networkId]);
    }

    template <class T>
    T* NetworkedEntitiesManager::TryGetComponent(NetworkEntityId networkId)
    {
        if(!HasNetworkedEntity(networkId))
            throw std::runtime_error("Cannot get component of a non existing networked entity");
        return m_registry->try_get<T>(m_networkedEntities[networkId]);
    }
}
