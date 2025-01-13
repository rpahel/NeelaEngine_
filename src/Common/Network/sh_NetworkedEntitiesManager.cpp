#include "Common/Network/sh_NetworkedEntitiesManager.h"

#include "Neela/Components/NameComponent.h"
#include "Neela/Components/TransformComponent.h"

namespace Neela
{
    struct NameComponent;
}

namespace ACNet
{
    NetworkedEntitiesManager::NetworkedEntitiesManager(entt::registry* registry) :
    m_registry(registry)
    {
        registry->on_construct<NetworkComponent>().connect<&NetworkedEntitiesManager::InternalOnNetworkedConstruct>(this);
        registry->on_destroy<NetworkComponent>().connect<&NetworkedEntitiesManager::InternalOnNetworkedDestruct>(this);
    }

    void NetworkedEntitiesManager::InternalOnNetworkedConstruct(entt::registry& registry, entt::entity entity)
    {
        NetworkComponent& networkComponent = registry.get<NetworkComponent>(entity);
        
        OnNetworkedConstruct(entt::handle{registry, entity}, networkComponent);
        
        Neela::NameComponent& nameComponent = registry.get<Neela::NameComponent>(entity);
        nameComponent.SetName(FormatNetworkedEntityName(nameComponent.GetName(), networkComponent.id));
    }

    void NetworkedEntitiesManager::InternalOnNetworkedDestruct(entt::registry& registry, entt::entity entity)
    {
        OnNetworkedDestruct(entt::handle{registry, entity}, registry.get<NetworkComponent>(entity));
    }

    bool NetworkedEntitiesManager::HasNetworkedEntity(ACNet::NetworkEntityId networkId)
    {
        assert(m_registry);
        return m_networkedEntities.contains(networkId);
    }

    entt::handle NetworkedEntitiesManager::GetNetworkedEntity(ACNet::NetworkEntityId networkId)
    {
        if(!HasNetworkedEntity(networkId))
            throw std::runtime_error("Can't get networked entity with unused NetworkId");
        return entt::handle{*m_registry, m_networkedEntities[networkId]};
    }
}
