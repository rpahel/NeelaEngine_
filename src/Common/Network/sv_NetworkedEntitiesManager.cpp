#include "Common/Network/sv_NetworkedEntitiesManager.h"

#include "Common/Agriculture/sv_AutomatedComponent.h"
#include "Neela/Components/NameComponent.h"
#include "Neela/Components/TransformComponent.h"

namespace ACNet
{
    SV_NetworkedEntitiesManager::SV_NetworkedEntitiesManager(entt::registry* registry,
        SV_MessageHandler* messageHandler) :
        NetworkedEntitiesManager(registry),
        m_messageHandler(messageHandler),
        m_farmPlotObserver(
            *registry, entt::collector.group<NetworkComponent, Neela::TransformComponent, ACGame::FarmPlotComponent>()),
        m_genericInteractablesObserver(
            *registry, entt::collector.group<NetworkComponent, Neela::TransformComponent, ACGame::GenericInteractableComponent>())
    {
        assert(m_messageHandler);
    }

    void SV_NetworkedEntitiesManager::Tick()
    {
        m_farmPlotObserver.each([&](entt::entity entity)
        {
            NetworkComponent& networkComponent = m_registry->get<NetworkComponent>(entity);
            Neela::TransformComponent& transformComponent = m_registry->get<Neela::TransformComponent>(entity);
            ACGame::FarmPlotComponent& farmPlotComponent = m_registry->get<ACGame::FarmPlotComponent>(entity);
            
            SV_SpawnFarmPlotPacket packet;
            packet.automatedFarmPlot = m_registry->any_of<ACGame::AutomatedComponent>(entity);
            packet.farmPlotPosition = transformComponent.m_Transform->GetAbsolutePosition();
            packet.farmPlotState = FarmPlotStatePacket(networkComponent.id, farmPlotComponent);

            m_messageHandler->SendMessageToClients(packet, ENET_PACKET_FLAG_RELIABLE);
        });

        m_genericInteractablesObserver.each([&](entt::entity entity)
        {
            NetworkComponent& networkComponent = m_registry->get<NetworkComponent>(entity);
            Neela::TransformComponent& transformComponent = m_registry->get<Neela::TransformComponent>(entity);
            ACGame::GenericInteractableComponent& genericInteractable = m_registry->get<ACGame::GenericInteractableComponent>(entity);

            SV_SpawnGenericInteractablePacket packet;
            packet.instancePacket = GenericInteractableInstancePacket(networkComponent.id, genericInteractable.type, transformComponent.m_Transform->GetAbsolutePosition());
            m_messageHandler->SendMessageToClients(packet, ENET_PACKET_FLAG_RELIABLE);
        });
    }

    void SV_NetworkedEntitiesManager::OnNetworkedConstruct(entt::handle handle, NetworkComponent& networkComponent)
    {
        networkComponent.id = m_nextNetworkId;
        m_networkedEntities.emplace(networkComponent.id, handle.entity());
        m_nextNetworkId++;
    }

    void SV_NetworkedEntitiesManager::OnNetworkedDestruct(entt::handle handle, NetworkComponent& networkComponent)
    {
        SV_DestroyEntityPacket packet;
        packet.id = networkComponent.id;
        
        m_networkedEntities.erase(networkComponent.id);

        m_messageHandler->SendMessageToClients(packet, ENET_PACKET_FLAG_RELIABLE);
    }
}
