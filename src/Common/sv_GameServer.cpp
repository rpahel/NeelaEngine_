#include "Neela/Essentials/Core.h"
#include "Common/sh_constants.h"
#include "Common/sv_GameServer.h"

#include <iostream>
#include <fmt/core.h>
#include <fmt/color.h>

#include "Common/EntityCreators/sh_EntityCreator.h"
#include "Common/Agriculture/sh_FarmPlotComponent.h"
#include "Common/Agriculture/sv_AutomatedComponent.h"
#include "Common/Agriculture/sv_AgricultureSystem.h"
#include "Common/Inputs/sv_PlayerInputsSystem.h"
#include "Neela/Components/TransformComponent.h"
#include "Neela/Essentials/EntityCreator.h"
#include "Common/Character/cl_CharacterComponent.h"
#include "Common/Interaction/sh_InteractionCallbackComponent.h"
#include "Common/Network/sv_DesyncComponent.h"
#include "Common/Spawner/sv_SpawnerComponent.h"

namespace ACNet
{
	GameServer::GameServer() :
		m_messageHandler(*this, m_clientsContainer),
		m_networkManager(*this, m_messageHandler, m_clientsContainer),
		m_networkedEntitiesManager(&m_registry, &m_messageHandler),
		m_nextGameTick(0.),
		m_nextNetworkTick(0.)
	{
	}

	void GameServer::StartServer()
	{
		if (IsRunning())
		{
			ACLogError(LogSource::Server, "Server already running. Can't start it.\n");
			throw std::runtime_error("[SERVER] Server already running. Can't start it.");
		}

		m_networkManager.StartServer();
		if (IsRunning())
		{
			InitializeGame();
		}
	}

	void GameServer::HandleTick()
	{
		if (!IsRunning())
			return;

		double gameTime = Neela::Core::GameTime();

		m_networkManager.HandleNetworkEvents();
		m_networkedEntitiesManager.Tick();

		while (gameTime >= m_nextGameTick)
		{
			GameTick();

			m_nextGameTick += ACGame::GameTickDelay;
		}

		while (gameTime >= m_nextNetworkTick)
		{
			NetworkTick();

			m_nextNetworkTick += SV_NetworkTickDelay;
		}
	}

	bool GameServer::IsRunning() const
	{
		return m_networkManager.IsServerStarted();
	}

	void GameServer::OnNewPlayerJoined(SV_Client* newPlayerClient)
	{
		assert(newPlayerClient);

		m_messageHandler.SendMessageToClient(newPlayerClient->clientPeer, BuildGameStatePacket(), ENET_PACKET_FLAG_RELIABLE);

		SV_PlayerJoinGamePacket joinedPacket;
		joinedPacket.attachedClientId = newPlayerClient->clientId;
		joinedPacket.appearance = newPlayerClient->appearance;
		m_messageHandler.SendMessageToClient(newPlayerClient->clientPeer, joinedPacket, ENET_PACKET_FLAG_RELIABLE);

		if (m_clientsContainer.GetClients().size() > 1)
		{
			SV_ExistingPlayersPacket existingPacket;
			for (auto& i : m_clientsContainer.GetClients())
			{
				if (i.clientId == newPlayerClient->clientId)
					continue;

				auto& data = existingPacket.players.emplace_back();
				data.clientId = i.clientId;
				data.appearance = i.appearance;
				data.userName = i.username;
			}
			m_messageHandler.SendMessageToClient(newPlayerClient->clientPeer, existingPacket, ENET_PACKET_FLAG_RELIABLE);

			joinedPacket.userName = newPlayerClient->username;
			m_messageHandler.SendMessageToClients(joinedPacket, ENET_PACKET_FLAG_RELIABLE, newPlayerClient);
		}
	}

	void GameServer::GameTick()
	{
		if (m_clientsContainer.GetInGameClientsCount() == 0)
			return;

		ACGame::AgricultureSystem::Apply(m_registry, m_messageHandler, &m_gameData, ACGame::GameTickDelay);
		ACGame::PlayerInputsSystem::ProcessPlayerInputs(&m_clientsContainer, &m_networkedEntitiesManager, &m_messageHandler, &m_gameData);
	}

	void GameServer::NetworkTick()
	{
		if (m_clientsContainer.GetInGameClientsCount() == 0)
			return;

		ServerSendPlayerPositions();

		ServerSendDesyncInfos();
		
		if(m_gameData.moneyChangedCount > 0)
			ServerSendMoneyUpdate();

		m_NetworkTickIndex++;
	}

	void GameServer::InitializeGame()
	{
		m_nextGameTick = Neela::Core::GameTime();
		m_nextNetworkTick = m_nextGameTick;

		m_gameData.SetMoney(ACGame::StartMoneyCount);
		
		m_gridData.SetSize(50, 50);
		m_gridData.GenerateRandom();

		Neela::Vector2f startPos(-15,-9);
		startPos *= ACGame::CellSize;
		const std::uint8_t spawnerSize = 8;
		const std::uint8_t interval = spawnerSize + 1;
		for (std::size_t i = 0; i < 4; ++i)
		{
			SpawnServerFarmPlotSpawner(static_cast<ACGame::FarmPlantType>(i+1), startPos + Neela::Vector2i(i % 2, i/2) * ACGame::CellSize * interval,
				spawnerSize, spawnerSize);
		}
	
		SpawnServerFarmPlotSpawner(static_cast<ACGame::FarmPlantType>(5), startPos + Neela::Vector2i(2, 0) * ACGame::CellSize * interval,
			10, 20);
		
		for (int i = 0; i < 3; ++i)
		{
			SpawnServerFarmPlot( startPos + Neela::Vector2f(i, -1) * ACGame::CellSize,
				ACGame::FarmPlotState::Grown, ACGame::FarmPlantType::Carrot);
		}
	}

	SV_GameStatePacket GameServer::BuildGameStatePacket()
	{
		SV_GameStatePacket gameStatePacket;
		gameStatePacket.gridData = m_gridData;
		gameStatePacket.money = m_gameData.money;
		
		{ // Farm Plots
			auto view = m_registry.view<ACGame::FarmPlotComponent, Neela::TransformComponent, NetworkComponent>(entt::exclude<ACGame::AutomatedComponent>);
			for (auto&& [entity, farmPlot, transform, network] : view.each())
			{
				gameStatePacket.farmPlots.emplace_back(
					FarmPlotStatePacket(network.id, farmPlot),
					transform.m_Transform->GetAbsolutePosition());
			}
		}
		
		{ // Auto farm plots
			auto view = m_registry.view<ACGame::AutomatedComponent, ACGame::FarmPlotComponent, Neela::TransformComponent, NetworkComponent>();
			for (auto&& [entity, farmPlot, transform, network] : view.each())
			{
				gameStatePacket.autoFarmPlots.emplace_back(
					FarmPlotStatePacket(network.id, farmPlot.state, farmPlot.plantType),
					transform.m_Transform->GetAbsolutePosition());
			}
		}

		{ // Generic interactables
			auto view = m_registry.view<Neela::TransformComponent, NetworkComponent, ACGame::GenericInteractableComponent, ACGame::InteractionCallbackComponent>();
			for (auto && [entity, transform, network, genericInteractable, interactionCallback] : view.each())
			{
				gameStatePacket.genericInteractables.emplace_back(
					network.id,
					genericInteractable.type,
					transform.m_Transform->GetAbsolutePosition());
			}
		}
		return gameStatePacket;
	}
	
	void GameServer::SpawnServerFarmPlot(Neela::Vector2f position, ACGame::FarmPlotState state,
		ACGame::FarmPlantType plantType)
	{
		entt::handle handle = ACGame::EntityCreator::CreatePlotFarmEntity(m_registry, "FarmPlot", position,
			state, plantType);
		if(ACGame::FarmPlantConfigs[plantType].automated)
			handle.emplace<ACGame::AutomatedComponent>();
		handle.emplace<NetworkComponent>();
	}

	void GameServer::SpawnServerAutomatedFarmPlot(Neela::Vector2f position, ACGame::FarmPlotState state,
		ACGame::FarmPlantType plantType)
	{
		entt::handle handle = ACGame::EntityCreator::CreatePlotFarmEntity(m_registry, "FarmPlot", position,
			state, plantType);
		handle.emplace<ACGame::AutomatedComponent>();
		handle.emplace<NetworkComponent>();
	}

	entt::handle GameServer::SpawnServerGenericInteractable(Neela::Vector2f position,
	                                                        ACGame::GenericInteractableEntitiesType type,
	                                                        const ACGame::InteractionCallback& callback)
	{
		entt::handle handle = ACGame::EntityCreator::CreateGenericInteractionCallbackEntity(m_registry, "GenericInteractable", position, type);
		handle.emplace<ACGame::InteractionCallbackComponent>().interactionCallback = callback;
		handle.emplace<NetworkComponent>();
		return handle;
	}

	void GameServer::SpawnServerFarmPlotSpawner(ACGame::FarmPlantType plantType, const Neela::Vector2f& position,
		std::uint8_t columnCount, std::uint8_t rowCount)
	{
		entt::handle carrotSpawner = SpawnServerGenericInteractable(position, ACGame::GenericInteractableEntitiesType::Buy,
			[plantType, this](entt::handle handle, std::uint8_t clientId)
			{
				if(ACGame::SpawnerComponent* spawnerComponent = handle.try_get<ACGame::SpawnerComponent>())
				{
					if(spawnerComponent->CanSpawn() && m_gameData.HasMoney(spawnerComponent->spawnPrice))
					{
						SpawnServerFarmPlot(spawnerComponent->GetNextSpawnPos(), ACGame::FarmPlotState::Empty, plantType);
						m_gameData.RemoveMoney(spawnerComponent->spawnPrice);
						spawnerComponent->spawnedCount++;

						if(!spawnerComponent->CanSpawn())
						{
							Neela::Vector2f spawnPosition = handle.get<Neela::TransformComponent>().m_Transform->GetAbsolutePosition();
							SpawnServerGenericInteractable(spawnPosition + Neela::Vector2f(1,0) * ACGame::CellSize,
								ACGame::GenericInteractableEntitiesType::Upgrade,
								[plantType, this](entt::handle, std::uint8_t clientId)
	                               {
		                               ACGame::FarmPlantConfig& farmPlantConfig = ACGame::FarmPlantConfigs[plantType];
		                               if(m_gameData.HasMoney(farmPlantConfig.upgradePrice))
		                               {
			                               m_gameData.RemoveMoney(farmPlantConfig.upgradePrice);
			                               farmPlantConfig.upgradePrice *= 2;
			                               farmPlantConfig.harvestGain *= 2;
		                               }
	                               });

							SpawnServerGenericInteractable(spawnPosition + Neela::Vector2f(2,0) * ACGame::CellSize,
								ACGame::GenericInteractableEntitiesType::Automate,
								[plantType, this](entt::handle handle, std::uint8_t clientId)
								{
									ACGame::FarmPlantConfig& farmPlantConfig = ACGame::FarmPlantConfigs[plantType];
									if(farmPlantConfig.automated)
										return;
									if(m_gameData.HasMoney(farmPlantConfig.automationPrice))
									{
										m_gameData.RemoveMoney(farmPlantConfig.automationPrice);
										farmPlantConfig.automated = true;
									}
								});
						}
					}
				}
			});
		ACGame::SpawnerComponent& spawnerComponent = carrotSpawner.emplace<ACGame::SpawnerComponent>();
		spawnerComponent.columnCount = columnCount;
		spawnerComponent.rowCount = rowCount;
		spawnerComponent.spawnPrice = ACGame::FarmPlantConfigs[plantType].spawnPrice;
		spawnerComponent.topLeftSpawnZone = position + Neela::Vector2f(0, 1) * ACGame::CellSize;
	}

	void GameServer::ServerSendPlayerPositions()
	{
		for (auto& clientA : m_clientsContainer.GetClients())
		{
			SV_PlayerPositionsPacket packet;
			for (auto& clientB : m_clientsContainer.GetClients())
				packet.players.push_back({ clientB.clientId, clientB.absolutePos });

			packet.lastInputIndex = clientA.lastInputIndex;
			packet.tickIndex = m_NetworkTickIndex;

			m_messageHandler.SendMessageToClient(&clientA, packet, 0);
		}
	}

	void GameServer::ServerSendDesyncInfos()
	{
		SV_FarmPlotsUpdate farmPlotsUpdate;
		
		auto view = m_registry.view<NetworkComponent, ACGame::DesyncComponent>();
		for (auto && [entity, networkComponent] : view.each())
		{
			if(ACGame::FarmPlotComponent* farmPlot = m_registry.try_get<ACGame::FarmPlotComponent>(entity))
			{
				farmPlotsUpdate.farmPlotStates.emplace_back(networkComponent.id, *farmPlot);
			}

			m_registry.remove<ACGame::DesyncComponent>(entity);
		}

		if(!farmPlotsUpdate.farmPlotStates.empty())
			m_messageHandler.SendMessageToClients(farmPlotsUpdate, ENET_PACKET_FLAG_RELIABLE);
	}

	void GameServer::ServerSendMoneyUpdate()
	{
		SV_MoneyUpdatePacket packet;
		packet.money = m_gameData.money;

		std::uint8_t flags = 0;
		if (m_gameData.moneyChangedCount < 3)
			flags = ENET_PACKET_FLAG_RELIABLE;
		m_messageHandler.SendMessageToClients(packet, flags);

		m_gameData.moneyChangedCount = 0;
	}
}
