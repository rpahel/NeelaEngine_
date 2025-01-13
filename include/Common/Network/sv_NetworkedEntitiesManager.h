#pragma once
#include <entt/entity/registry.hpp>

#include "sh_NetworkedEntitiesManager.h"
#include "Common/Network/sv_MessageHandler.h"

namespace ACNet
{
    class SV_NetworkedEntitiesManager : public NetworkedEntitiesManager
    {
    private:
        entt::observer m_farmPlotObserver;
        entt::observer m_genericInteractablesObserver;
        
        SV_MessageHandler* m_messageHandler;

        NetworkEntityId m_nextNetworkId = 0;
        
    public:
        //==== Constructors ====
        SV_NetworkedEntitiesManager(entt::registry* registry, SV_MessageHandler* messageHandler);

        //==== Tick ====
        void Tick() override;

    protected:
        void OnNetworkedConstruct(entt::handle handle, NetworkComponent& networkComponent) override;
        void OnNetworkedDestruct(entt::handle handle, NetworkComponent& networkComponent) override;
    };
}
