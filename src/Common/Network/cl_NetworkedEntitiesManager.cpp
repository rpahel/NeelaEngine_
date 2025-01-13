#include "Common/Network/cl_NetworkedEntitiesManager.h"

namespace ACNet
{
    CL_NetworkedEntitiesManager::CL_NetworkedEntitiesManager(entt::registry* registry) :
    NetworkedEntitiesManager(registry)
    {
    }

    void CL_NetworkedEntitiesManager::Tick()
    {
    }

    void CL_NetworkedEntitiesManager::OnNetworkedConstruct(entt::handle handle, NetworkComponent& networkComponent)
    {
        m_networkedEntities.emplace(networkComponent.id, handle.entity());
    }

    void CL_NetworkedEntitiesManager::OnNetworkedDestruct(entt::handle handle, NetworkComponent& networkComponent)
    {
        m_networkedEntities.erase(networkComponent.id);
    }
}
