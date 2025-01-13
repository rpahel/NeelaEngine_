#pragma once

#include "Common/Clients/sv_Client.h"
#include "Common/Grid/sh_GridData.h"
#include "Common/Network/sv_NetworkManager.h"
#include "Network/sv_MessageHandler.h"
#include <entt/entt.hpp>

#include "sh_GameData.h"
#include "Agriculture/sh_FarmPlotComponent.h"
#include "Common/Network/sv_NetworkedEntitiesManager.h"
#include "Interaction/sh_InteractionCallbackComponent.h"

namespace ACNet
{
	class GameServer
	{
	private:
		entt::registry m_registry;

		SV_ClientsContainer m_clientsContainer;
		SV_MessageHandler m_messageHandler;
		SV_NetworkManager m_networkManager;
		SV_NetworkedEntitiesManager m_networkedEntitiesManager;

		double m_nextGameTick;
		double m_nextNetworkTick;
		std::uint32_t m_NetworkTickIndex = 1;

		ACGame::GameData m_gameData;
		ACGame::GridData m_gridData;


	public:
		GameServer();

		void StartServer();
		void HandleTick();
		bool IsRunning() const;

		void OnNewPlayerJoined(SV_Client* newPlayerClient);
		
	private:
		void GameTick();
		void NetworkTick();

		void InitializeGame();
		SV_GameStatePacket BuildGameStatePacket();

		void SpawnServerFarmPlot(Neela::Vector2f position, ACGame::FarmPlotState state, ACGame::FarmPlantType plantType);
		void SpawnServerAutomatedFarmPlot(Neela::Vector2f position, ACGame::FarmPlotState state,
		                                  ACGame::FarmPlantType plantType);
		entt::handle SpawnServerGenericInteractable(Neela::Vector2f position,
		                                            ACGame::GenericInteractableEntitiesType type,
		                                            const ACGame::InteractionCallback& callback);
		void SpawnServerFarmPlotSpawner(ACGame::FarmPlantType plantType, const Neela::Vector2f& position,
				std::uint8_t columnCount = 8, std::uint8_t rowCount = 8);

		void ServerSendPlayerPositions();
		void ServerSendDesyncInfos();
		void ServerSendMoneyUpdate();
	};
}
