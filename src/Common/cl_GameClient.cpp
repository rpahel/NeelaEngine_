#include "Neela/NeelaCore.h"
#include "Common/cl_GameClient.h"
#include "Common/sh_constants.h"

#include <iostream>
#include <fmt/color.h>

#include "Common/EntityCreators/cl_ClientEntityCreator.h"
#include "Common/cl_TextureConstants.h"
#include "Common/EntityCreators/sh_EntityCreator.h"
#include "Common/Interaction/sh_InteractableComponent.h"
#include "Common/sh_logger.h"
#include "Common/Agriculture/cl_FarmPlotHelpers.h"
#include "Common/Grid/cl_RenderableGrid.h"
#include "Common/Character/cl_CharacterComponent.h"
#include "Common/Inputs/cl_PlayerControllerComponent.h"
#include "Common/Inputs/cl_PlayerControllerSystem.h"
#include "Common/Interaction/cl_InteractionTooltipComponent.h"
#include "Common/Interface/cl_LabelComponent.h"

namespace ACNet
{
	GameClient::GameClient() :
		m_messageHandler(*this, m_clientsContainer),
		m_networkManager(*this, m_messageHandler, m_clientsContainer),
		m_networkedEntitiesManager(&Neela::Core::World()),
		m_tooltipSystem(&Neela::Core::World()),
		m_labelSystem(&Neela::Core::World()),
		m_nextGameTick(0.),
		m_isRunning(false),
		m_gridData(50, 50)
	{
	}

	GameClient::State GameClient::GetState() const
	{
		return m_state;
	}

	void GameClient::Initialize()
	{
		m_state = State::Offline;
	}

	bool GameClient::RequestServerConnection(std::string targetIpAddress)
	{
		m_state = State::RequestingServerConnection;
		return m_networkManager.SendServerConnectionRequest(std::move(targetIpAddress));
	}

	bool GameClient::IsConnectedToServer() const
	{
		return m_networkManager.IsConnectedToServer();
	}

	void GameClient::RequestGameJoin(const std::string& username, const ACGame::CharacterAppearance& appearance)
	{
		if (m_isRunning)
			throw std::runtime_error("[CLIENT] Client has already joined game. Can't request to join");
		if (!IsConnectedToServer())
			throw std::runtime_error("[CLIENT] Client not connected to a server. Connect it first");

		m_state = State::RequestingGameConnection;
		
		m_clientsContainer.AddClient(username, true);
		// TODO: send more infos ? customization ?
		CL_PlayerJoinGameRequestPacket packet;
		packet.username = username;
		packet.appearance = appearance;
		m_messageHandler.SendMessageToServer(packet, ENET_PACKET_FLAG_RELIABLE);

		ACLog(LogSource::Client, LogType::InfoNetwork, "Sent request for joining the game as '{0}'.\n", username);
	}

	void GameClient::OnLocalPlayerGameJoined(std::uint8_t givenClientId, const ACGame::CharacterAppearance& appearance)
	{
		if (m_isRunning)
			throw std::runtime_error("[CLIENT] Client already joined the game.\n");
		if (!IsConnectedToServer())
			throw std::runtime_error("[CLIENT] Client not connected to a server. Connect it first.\n");

		m_state = State::ConnectedToGame;
		
		// Get camera entity
		auto cameraView = Neela::Core::World().view<Neela::TransformComponent, Neela::CameraComponent>();
		for (auto&& [entity, transformComponent, cameraComponent] : cameraView.each())
		{
			m_cameraEntity = entity;
			m_cameraTransform = transformComponent.m_Transform;
			break;
		}
		assert(Neela::Core::World().valid(m_cameraEntity) && m_cameraTransform);
		m_labelSystem.SetCameraTransform(m_cameraTransform);
		
		// Setup local client and character
		m_clientsContainer.GetLocalClient()->clientId = givenClientId;
		CreateLocalPlayerCharacter(givenClientId, appearance);

		// Setup grid
		ACGame::ClientEntityCreator::CreateClientGridEntity(Neela::Core::World(), "Grid",
			TopLeftWindowPos, &m_gridData);

		Neela::InputsManager::BindMouseButton(Neela::EMouseButton::LMB, "Interact");
		Neela::InputsManager::BindKey(SDL_KeyCode::SDLK_SPACE, "Interact");
		Neela::InputsManager::BindAction("Interact", [&](bool pressed)
			{
				OnInteractionMouseButtonPressed(pressed);
			});

		// Setup bank interface
		Neela::EntityCreator::CreateInterfaceEntity(
			Neela::Core::World(),
			"Bank",
			[this]()
			{
				ImGuiWindowFlags flags = 0;
				flags |= ImGuiWindowFlags_NoBackground;
				flags |= ImGuiWindowFlags_NoDecoration;
				flags |= ImGuiWindowFlags_NoMove;
				flags |= ImGuiWindowFlags_NoSavedSettings;

				ImGui::SetNextWindowSize(ImVec2(ACGame::WindowWidth - 20, 100));
				ImGui::SetNextWindowPos(ImVec2(10,10));
				if(ImGui::Begin("Bank", nullptr, flags))
				{
					ImGui::SetWindowFontScale(1.2f);
					ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Money: %d", m_gameData.money);
				}
				ImGui::End();
				return false;
			});

		m_isRunning = true;
		m_nextGameTick = Neela::Core::GameTime();

		ACLog(LogSource::Client, LogType::Debug, "Created character entity.\n");

		ACLog(LogSource::Client, LogType::InfoGameState, "Client joined the game.\n");
	}

	void GameClient::OnPlayerGameJoined(std::uint8_t givenClientId, const std::string& playerName, const ACGame::CharacterAppearance& appearance)
	{
		if (!IsConnectedToServer())
			throw std::runtime_error("[CLIENT] Client not connected to a server. Connect it first.\n");

		m_clientsContainer.AddClient(givenClientId, playerName, false);
		CreatePlayerCharacter(givenClientId, playerName, appearance);
	}

	void GameClient::UpdatePlayerPositions(const SV_PlayerPositionsPacket& packet)
	{
		if (packet.players.empty())
			return;

		if (m_interpBuffer.empty())
		{
			// Position queue vide ? (soit d�but de game, soit grosse perte de paquet)
			// On initialise notre position queue avec le m�me paquet plusieurs fois
			std::uint32_t tickIndex = packet.tickIndex;
			for (std::size_t i = 0; i < TargetInterpBufferSize; ++i)
			{
				// Correction du tick index
				m_interpBuffer.push_back({ tickIndex - (TargetInterpBufferSize - i - 1), packet.players });
			}
		}
		else
			m_interpBuffer.push_back({ packet.tickIndex, packet.players });

		for (auto& playerData : packet.players)
		{
			CL_Client* client = m_clientsContainer.GetClient(playerData.clientId);
			if (!client)
				continue;

			client->absolutePos = playerData.position;

			if (!client->handle.valid())
				continue;

			Neela::Transform& transform = *client->handle.get<Neela::TransformComponent>().m_Transform;

			if (client->isLocalPlayer)
			{
				while (!client->predictedInputs.empty() && client->predictedInputs.front().first.index <= packet.lastInputIndex)
					client->predictedInputs.erase(client->predictedInputs.begin());

				if (client->predictedInputs.empty())
					continue;

				const Neela::Vector2f diff = client->predictedInputs.front().second - playerData.position;

				if (diff.GetMagnitudeSquared() > PredictionToleranceSqrd)
				{
					//const Neela::Vector2f previousPos = transform.GetAbsolutePosition();

					//ACLogWarning(LogSource::Client, "RECONCILIATION\n");
					transform.SetAbsolutePosition(playerData.position);

					for (auto& predictedInput : client->predictedInputs)
						ACGame::PlayerControllerSystem::ProcessMovements(client->handle, predictedInput.first.inputs, false);

					//const Neela::Vector2f newPos = transform.GetAbsolutePosition();

					OffsetPlayerCharacterSprite(client->handle, diff);
				}

				continue;
			}

			const Neela::Vector2f dV = client->absolutePos - transform.GetAbsolutePosition();

			transform.SetAbsolutePosition(client->absolutePos);

			if (ACGame::CharacterComponent* charaComp = client->handle.try_get<ACGame::CharacterComponent>())
			{
				// Les animations de deplacement horizontal ont la priorite sur celles en vertical.
				if (dV.X != 0)
				{
					if (dV.X > 0)
					{
						charaComp->PlayAnimation("WalkRight");
						charaComp->m_CurrentOrientation = ACGame::CharacterOrientation::Right;
					}
					else
					{
						charaComp->PlayAnimation("WalkLeft");
						charaComp->m_CurrentOrientation = ACGame::CharacterOrientation::Left;
					}
				}
				else if (dV.Y != 0)
				{
					if (dV.Y < 0)
					{
						charaComp->PlayAnimation("WalkUp");
						charaComp->m_CurrentOrientation = ACGame::CharacterOrientation::Up;
					}
					else
					{
						charaComp->PlayAnimation("WalkDown");
						charaComp->m_CurrentOrientation = ACGame::CharacterOrientation::Down;
					}
				}
				else
				{
					switch (charaComp->m_CurrentOrientation)
					{
						case ACGame::CharacterOrientation::Up:
							charaComp->PlayAnimation("IdleUp");
							break;

						case ACGame::CharacterOrientation::Down:
							charaComp->PlayAnimation("IdleDown");
							break;

						case ACGame::CharacterOrientation::Left:
							charaComp->PlayAnimation("IdleLeft");
							break;

						case ACGame::CharacterOrientation::Right:
							charaComp->PlayAnimation("IdleRight");
							break;

						default:
							break;
					}
				}
			}

			client->handle.get<Neela::TransformComponent>().m_Transform->SetAbsolutePosition(client->absolutePos);
		}
	}

	ACGame::GameData& GameClient::GetGameData()
	{
		return m_gameData;
	}

	const ACGame::GameData& GameClient::GetGameData() const
	{
		return m_gameData;
	}

	void GameClient::HandleTick()
	{
		m_networkManager.HandleNetworkEvents();
		
		switch (m_state)
		{
		case State::Offline:
			// do nothing, waiting for a trigger of "RequestServerConnection"
			break;
		case State::RequestingServerConnection:
			{
				if(m_networkManager.IsConnectedToServer())
					m_state = State::ConnectedToServer;
				else if(m_networkManager.DoesConnectionFailed())
					m_state = State::ConnectionFailed;
				else if (m_networkManager.IsConnectionInProgress())
					// do nothing, wait for connection result
						break;
				break;
			}
		case State::ConnectedToServer:
			// do nothing, waiting for a trigger of "RequestGameJoin"
			break;
		case State::RequestingGameConnection:
			// do nothing, wait for a connection result
			break;
		case State::ConnectedToGame:
			{
				Tick();

				double gameTime = Neela::Core::GameTime();
				while (gameTime >= m_nextGameTick)
				{
					GameTick();
					m_nextGameTick += ACGame::GameTickDelay;
				}
				break;
			}
		case State::ConnectionFailed:
			break;
		case State::None:
		default: throw;
		}
	}

	bool GameClient::IsRunning() const
	{
		return m_state != State::ConnectionFailed && Neela::WindowsManager::IsMainWindowOpen();
	}

	CL_NetworkedEntitiesManager& GameClient::GetNetworkedEntitiesManager()
	{
		return m_networkedEntitiesManager;
	}

	void GameClient::SetGameState(const SV_GameStatePacket& gameState)
	{
		// update grid
		SetGridData(gameState.gridData);
		m_gameData.SetMoney(gameState.money);

		// Update / Spawn farm plots
		for (std::size_t i = 0; i < gameState.farmPlots.size(); ++i)
		{
			const FarmPlotInstancePacket& farmPlotInstancePacket = gameState.farmPlots[i];
			NetworkEntityId networkEntityId = farmPlotInstancePacket.farmPlotStatePacket.networkId;

			if (m_networkedEntitiesManager.HasNetworkedEntity(networkEntityId))
			{
				farmPlotInstancePacket.farmPlotStatePacket.Apply(m_networkedEntitiesManager.GetComponent<ACGame::FarmPlotComponent>(networkEntityId));
				m_networkedEntitiesManager.GetComponent<Neela::TransformComponent>(networkEntityId).m_Transform->SetAbsolutePosition(farmPlotInstancePacket.position);
			}
			else
			{
				SpawnClientFarmPlot(networkEntityId, farmPlotInstancePacket.position,
					farmPlotInstancePacket.farmPlotStatePacket.farmPlotState,
					farmPlotInstancePacket.farmPlotStatePacket.farmPlantType);
			}
		}

		// Update / Spawn auto farm plots
		for (std::size_t i = 0; i < gameState.autoFarmPlots.size(); ++i)
		{
			const FarmPlotInstancePacket& autoFarmPlotInstancePacket = gameState.autoFarmPlots[i];
			NetworkEntityId networkEntityId = autoFarmPlotInstancePacket.farmPlotStatePacket.networkId;

			if (m_networkedEntitiesManager.HasNetworkedEntity(networkEntityId))
			{
				autoFarmPlotInstancePacket.farmPlotStatePacket.Apply(m_networkedEntitiesManager.GetComponent<ACGame::FarmPlotComponent>(networkEntityId));
				m_networkedEntitiesManager.GetComponent<Neela::TransformComponent>(networkEntityId).m_Transform->SetAbsolutePosition(autoFarmPlotInstancePacket.position);
			}
			else
			{
				SpawnClientFarmPlot(networkEntityId, autoFarmPlotInstancePacket.position,
					autoFarmPlotInstancePacket.farmPlotStatePacket.farmPlotState,
					autoFarmPlotInstancePacket.farmPlotStatePacket.farmPlantType);
			}
		}

		// Spawn generic interactables
		for (const auto& packet : gameState.genericInteractables)
		{
			NetworkEntityId networkEntityId = packet.networkId;
			SpawnClientGenericInteractable(networkEntityId, packet.position, packet.type);
		}
	}

	const ACGame::GridData& GameClient::GetGridData() const
	{
		return m_gridData;
	}

	void GameClient::SetGridData(const ACGame::GridData& newGridData)
	{
		m_gridData = newGridData;
	}

	void GameClient::SpawnClientFarmPlot(NetworkEntityId id, Neela::Vector2f position,
		ACGame::FarmPlotState state, ACGame::FarmPlantType farmPlantType)
	{
		auto handle = ACGame::EntityCreator::CreatePlotFarmEntity(Neela::Core::World(), "FarmPlot", position, state, farmPlantType);
		handle.emplace<NetworkComponent>(id);

		// Set interactable
		handle.emplace<ACGame::InteractableComponent>();

		// Add sprite for farm plant
		{
			std::shared_ptr<Neela::Texture> farmPlantTexture = Neela::ResourcesManager::GetTexture(Neela::WindowsManager::GetMainWindow(), ASSET_CARROT_GROWING);
			std::shared_ptr<Neela::Sprite> farmPlantSprite = std::make_shared<Neela::Sprite>(farmPlantTexture);
			farmPlantSprite->SetRenderLayer(21);
			handle.emplace<Neela::GraphicsComponent>(farmPlantSprite);
			handle.get<Neela::TransformComponent>().m_Transform->SetAbsoluteScale(Neela::Vector2f::One() * ACGame::CellSize / farmPlantSprite->GetTexture()->GetSize().X);
		}

		// Create children sprite entity (farm plot visual)
		{
			std::shared_ptr<Neela::Texture> farmPlotTexture = Neela::ResourcesManager::GetTexture(Neela::WindowsManager::GetMainWindow(), ASSET_PLOT_FARM);
			std::shared_ptr<Neela::Transform> farmPlotSpriteTransform = std::make_shared<Neela::Transform>(handle.get<Neela::TransformComponent>().m_Transform.get());
			farmPlotSpriteTransform->SetAbsoluteScale(Neela::Vector2f::One() * ACGame::CellSize / farmPlotTexture->GetSize().X);

			entt::handle farmPlotSpriteEntity = Neela::EntityCreator::CreateSpriteEntity(Neela::Core::World(), FormatNetworkedEntityName("FarmPlantSprite", id),
				farmPlotTexture, std::make_shared<Neela::SpriteSheet>(), farmPlotSpriteTransform);
			farmPlotSpriteEntity.get<Neela::GraphicsComponent>().m_Renderable->SetRenderLayer(20);
		}

		ACGame::FarmPlotHelpers::UpdateFarmPlotVisual(handle, handle.get<ACGame::FarmPlotComponent>());
	}

	void GameClient::SpawnClientGenericInteractable(NetworkEntityId networkEntityId, const Neela::Vector2f& position,
		ACGame::GenericInteractableEntitiesType type)
	{
		entt::handle handle = ACGame::EntityCreator::CreateGenericInteractionCallbackEntity(Neela::Core::World(), "Interactable", position, type);
		handle.emplace<NetworkComponent>(networkEntityId);

		std::string texturePath = TILE_ASSET_STONE;
		switch (type)
		{
		case ACGame::GenericInteractableEntitiesType::Buy:
			texturePath = TILE_ASSET_DIRT;
			break;
		case ACGame::GenericInteractableEntitiesType::Upgrade:
			texturePath = TILE_ASSET_STONE;
			break;
		case ACGame::GenericInteractableEntitiesType::Automate:
			texturePath = TILE_ASSET_DIRT;
			break;
		default: throw;
		}
		handle.emplace<Neela::GraphicsComponent>(std::make_shared<Neela::Sprite>(Neela::ResourcesManager::GetTexture(Neela::WindowsManager::GetMainWindow(), texturePath)))
			.m_Renderable->SetRenderLayer(30);
		handle.get<Neela::TransformComponent>().m_Transform->SetAbsoluteScale(Neela::Vector2f::One() * 2.f);

		ACGame::InteractionTooltipComponent& tooltipComponent = handle.emplace<ACGame::InteractionTooltipComponent>();
		switch(type)
		{
		case ACGame::GenericInteractableEntitiesType::None:
			tooltipComponent.tooltip = "None";
			break;
		case ACGame::GenericInteractableEntitiesType::Buy:
			tooltipComponent.tooltip = "Buy a new farm plot";
			break;
		case ACGame::GenericInteractableEntitiesType::Upgrade:
			tooltipComponent.tooltip = "Multiply by 2 the plant harvest gain";
			break;
		case ACGame::GenericInteractableEntitiesType::Automate:
			tooltipComponent.tooltip = "Automate the plant's plantation and harvest";
			break;
		}
	}

	void GameClient::OnEntityDestroyReceived(NetworkEntityId id)
	{
		Neela::Core::World().destroy(m_networkedEntitiesManager.GetNetworkedEntity(id));
		ACLog(LogSource::Client, LogType::InfoGameState, "Destroyed entity with NetId '{0}'", id);
	}

	void GameClient::Tick()
	{
		LerpPositions();
		m_labelSystem.Apply();
	}

	void GameClient::GameTick()
	{
		if(Neela::Core::World().valid(m_cameraEntity) && m_cameraTransform)
			m_hoveredEntity = GetEntityAtMousePos(Neela::Core::World(), *m_cameraTransform).entity();
		else
			m_hoveredEntity = entt::null;

		ACGame::PlayerControllerSystem::ProcessPlayerInputs(m_clientsContainer, &m_messageHandler);
		ReducePlayerCharactersOffsets();

		m_tooltipSystem.Apply(Neela::InputsManager::GetMousePosition(), m_hoveredEntity);
	}

	void GameClient::CreateLocalPlayerCharacter(std::uint8_t givenClientId, const ACGame::CharacterAppearance& appearance)
	{
		auto defaultChar = ACGame::ClientEntityCreator::CreateClientPlayerCharacterEntity(Neela::Core::World(),
			"PlayerCharacter (" + std::to_string(+givenClientId) + ")");
		m_clientsContainer.GetLocalClient()->handle = defaultChar;
		m_clientsContainer.GetLocalClient()->appearance = appearance;
		defaultChar.get<ACGame::CharacterComponent>().m_Appearance = appearance;
		defaultChar.get<ACGame::CharacterComponent>().UpdateAppearance();

		ACGame::LabelComponent& labelComponent = defaultChar.emplace<ACGame::LabelComponent>();
		labelComponent.label = m_clientsContainer.GetLocalClient()->username;
		labelComponent.offset = Neela::Vector2f(0, -40);

		Neela::InputsManager::BindKey(SDL_KeyCode::SDLK_UP, "WalkUp");
		Neela::InputsManager::BindKey(SDL_KeyCode::SDLK_z, "WalkUp");
		Neela::InputsManager::BindKey(SDL_KeyCode::SDLK_DOWN, "WalkDown");
		Neela::InputsManager::BindKey(SDL_KeyCode::SDLK_s, "WalkDown");
		Neela::InputsManager::BindKey(SDL_KeyCode::SDLK_LEFT, "WalkLeft");
		Neela::InputsManager::BindKey(SDL_KeyCode::SDLK_q, "WalkLeft");
		Neela::InputsManager::BindKey(SDL_KeyCode::SDLK_RIGHT, "WalkRight");
		Neela::InputsManager::BindKey(SDL_KeyCode::SDLK_d, "WalkRight");

		Neela::InputsManager::BindAction("WalkUp",
			[defaultChar](bool isPressed)
			{
				if (!defaultChar.valid())
					return;

				if (ACGame::PlayerControllerComponent* conComp = defaultChar.try_get<ACGame::PlayerControllerComponent>())
					conComp->m_Inputs.m_Up = isPressed;
			}
		);

		Neela::InputsManager::BindAction("WalkDown",
			[defaultChar](bool isPressed)
			{
				if (!defaultChar.valid())
					return;

				if (ACGame::PlayerControllerComponent* conComp = defaultChar.try_get<ACGame::PlayerControllerComponent>())
					conComp->m_Inputs.m_Down = isPressed;
			}
		);

		Neela::InputsManager::BindAction("WalkLeft",
			[defaultChar](bool isPressed)
			{
				if (!defaultChar.valid())
					return;

				if (ACGame::PlayerControllerComponent* conComp = defaultChar.try_get<ACGame::PlayerControllerComponent>())
					conComp->m_Inputs.m_Left = isPressed;
			}
		);

		Neela::InputsManager::BindAction("WalkRight",
			[defaultChar](bool isPressed)
			{
				if (!defaultChar.valid())
					return;

				if (ACGame::PlayerControllerComponent* conComp = defaultChar.try_get<ACGame::PlayerControllerComponent>())
					conComp->m_Inputs.m_Right = isPressed;
			}
		);
	}

	void GameClient::CreatePlayerCharacter(std::uint8_t givenClientId, const std::string& playerName, const ACGame::CharacterAppearance& appearance)
	{
		ACNet::CL_Client* client = m_clientsContainer.GetClient(givenClientId);
		if (!client)
			return;

		client->appearance = appearance;
		entt::handle handle = ACGame::ClientEntityCreator::CreateClientCharacterEntity(Neela::Core::World(), "playerName (" + std::to_string(+givenClientId) + ")");
		handle.get<ACGame::CharacterComponent>().m_Appearance = client->appearance;
		handle.get<ACGame::CharacterComponent>().UpdateAppearance();
		ACGame::LabelComponent& labelComponent = handle.emplace<ACGame::LabelComponent>();
		labelComponent.label = playerName;
		labelComponent.offset = Neela::Vector2f(0, -40);
		client->handle = handle;
	}

	void GameClient::OnInteractionMouseButtonPressed(bool pressed)
	{
		if (!pressed)
			return;
		
		auto& registry = Neela::Core::World();

		if (!registry.valid(m_cameraEntity) || !registry.valid(m_hoveredEntity))
			return;

		if (NetworkComponent* networkComponent = registry.try_get<NetworkComponent>(m_hoveredEntity))
		{
			m_clientsContainer.GetLocalClient()->handle.get<ACGame::PlayerControllerComponent>().m_Inputs.m_interactedNetworkEntityId.emplace(networkComponent->id);
		}
	}

	entt::handle GameClient::GetEntityAtMousePos(entt::registry& registry, const Neela::Transform& camera)
	{
		const Neela::Vector2i mousePos = Neela::InputsManager::GetMousePosition();
		const Neela::Vector2f relativePos(mousePos);

		auto view = registry.view<Neela::TransformComponent, Neela::GraphicsComponent, ACGame::InteractableComponent>();
		for (auto&& [entity, transform, gfx] : view.each())
		{
			const Neela::Vector2f worldPos = camera.RelativeToAbsolutePosition(relativePos);
			const Neela::Vector2f localPos = transform.m_Transform->AbsoluteToRelativePosition(worldPos);

			const Neela::Rect bounds = gfx.m_Renderable->GetBounds();
			if (bounds.Contains(localPos))
			{
				return entt::handle{ registry, entity };
			}
		}

		return entt::handle{ registry, entt::null };
	}

	void GameClient::LerpPositions()
	{
		if (m_clientsContainer.GetClients().empty())
			return;

		if (m_interpBuffer.size() >= 2)
		{
			const std::pair<std::uint32_t, std::vector<PlayerData>>& from = m_interpBuffer[0];
			const std::pair<std::uint32_t, std::vector<PlayerData>>& to = m_interpBuffer[1];

			std::uint32_t packetDiff = to.first - from.first;

			float interpolationIncr = Neela::Core::DeltaTime() / ACNet::SV_NetworkTickDelay;
			interpolationIncr /= packetDiff;

			// Si on accumule trop de positions, on acc�l�re l�g�rement le facteur d'interpolation
			if (m_interpBuffer.size() >= ACNet::TargetInterpBufferSize)
				interpolationIncr *= 1.f + 0.2f * (m_interpBuffer.size() - ACNet::TargetInterpBufferSize);
			else
				interpolationIncr *= Neela::Maths::Max(1.f - 0.2f * (ACNet::TargetInterpBufferSize - m_interpBuffer.size()), 0.f);

			for (CL_Client& client : m_clientsContainer.GetMutableClients())
			{
				if (client.isLocalPlayer)
					continue;

				if (!client.handle || !client.handle.valid())
					continue;

				std::int8_t presentInBothSnapshots = -1;

				Neela::Vector2f fromPos;
				for (auto& fromPlayer : from.second)
				{
					if (fromPlayer.clientId == client.clientId)
					{
						presentInBothSnapshots++;
						fromPos = fromPlayer.position;
						break;
					}
				}

				Neela::Vector2f toPos;
				for (auto& toPlayer : to.second)
				{
					if (toPlayer.clientId == client.clientId)
					{
						presentInBothSnapshots++;
						toPos = toPlayer.position;
						break;
					}
				}

				if (presentInBothSnapshots != 1)
					continue;

				Neela::Transform& transform = *client.handle.get<Neela::TransformComponent>().m_Transform;
				transform.SetAbsolutePosition(Neela::Maths::Lerp(fromPos, toPos, m_interpTime));
			}

			m_interpTime += interpolationIncr;
			while (m_interpTime >= 1.f)
			{
				if (!m_interpBuffer.empty())
					m_interpBuffer.erase(m_interpBuffer.begin());
				m_interpTime -= 1.f;
			}
		}
	}

	void GameClient::OffsetPlayerCharacterSprite(entt::handle& playerCharacter, const Neela::Vector2f& offset)
	{
		if (!playerCharacter || !playerCharacter.valid())
			return;

		ACGame::CharacterComponent* charaComp = playerCharacter.try_get<ACGame::CharacterComponent>();
		if (!charaComp)
			return;

		for (entt::handle& entity : charaComp->m_AppearancePartEntities)
		{
			Neela::SpriteSheetComponent* ss = entity.try_get<Neela::SpriteSheetComponent>();

			if(!ss)
				continue;

			Neela::Sprite* sprite = ss->m_Sprite.get();
			if(!sprite)
				continue;

			sprite->SetOffset(offset);
		}
	}

	void GameClient::ReducePlayerCharactersOffsets()
	{
		for (auto [entity, charaComp] : Neela::Core::World().view<ACGame::CharacterComponent>().each())
		{
			for (auto& appEntity : charaComp.m_AppearancePartEntities)
			{
				Neela::SpriteSheetComponent* ss = appEntity.try_get<Neela::SpriteSheetComponent>();

				if (!ss)
					continue;

				Neela::Sprite* sprite = ss->m_Sprite.get();
				if (!sprite)
					continue;

				sprite->SetOffset(sprite->GetOffset() * 0.8f);
			}
		}
	}
}
