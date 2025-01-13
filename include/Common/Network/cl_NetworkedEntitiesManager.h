#pragma once
#include <entt/entt.hpp>

#include "sh_NetworkedEntitiesManager.h"
#include "Common/Network/sv_MessageHandler.h"

namespace ACNet
{
    class CL_NetworkedEntitiesManager : public NetworkedEntitiesManager
    {
    public:
        //==== Constructors ====
        CL_NetworkedEntitiesManager(entt::registry* registry);

        //==== Tick ====
        void Tick() override;

    protected:
        void OnNetworkedConstruct(entt::handle handle, NetworkComponent& networkComponent) override;
        void OnNetworkedDestruct(entt::handle handle, NetworkComponent& networkComponent) override;
    };
}
