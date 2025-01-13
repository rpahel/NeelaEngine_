#pragma once

#include "Clients/cl_Client.h"
#include "Grid/sh_GridData.h"
#include "Network/cl_MessageHandler.h"
#include "Network/cl_NetworkedEntitiesManager.h"
#include "Network/cl_NetworkManager.h"
#include "Neela/Graphics/Transform.h"
#include <entt/entity/registry.hpp>

#include "sh_GameData.h"
#include "Interaction/cl_TooltipSystem.h"
#include "Interface/cl_LabelSystem.h"

namespace ACNet
{
	class GameClient
	{
	public:
		enum class State : std::uint8_t
		{
			None = 0,
			Offline, // Default state
			RequestingServerConnection, // Currently requesting a connection to the server
			ConnectedToServer, // Connected to the server but not yet in the game
			RequestingGameConnection, // Currently connected to server and requesting a connection to the game
			ConnectedToGame, // Connected to the server and the game
			ConnectionFailed,
		};
		
	private:
		State m_state;
		
		CL_ClientsContainer m_clientsContainer;
		CL_MessageHandler m_messageHandler;
		CL_NetworkManager m_networkManager;
		CL_NetworkedEntitiesManager m_networkedEntitiesManager;

		ACGame::TooltipSystem m_tooltipSystem;
		ACGame::LabelSystem m_labelSystem;
		
		std::vector<std::pair<std::uint32_t, std::vector<PlayerData>>> m_interpBuffer;
		float m_interpTime = 0;
		double m_nextGameTick = 0;
		bool m_isRunning = false;

		ACGame::GameData m_gameData;
		ACGame::GridData m_gridData;

		entt::entity m_cameraEntity;
		std::shared_ptr<Neela::Transform> m_cameraTransform;
		entt::entity m_hoveredEntity; // entity hovered by the mouse

	public:
		GameClient();

		State GetState() const;

		void Initialize();

		bool RequestServerConnection(std::string targetIpAddress);
		bool IsConnectedToServer() const;

		void RequestGameJoin(const std::string& username, const ACGame::CharacterAppearance& appearance);
		void OnLocalPlayerGameJoined(std::uint8_t givenClientId, const ACGame::CharacterAppearance& appearance);
		void OnPlayerGameJoined(std::uint8_t givenClientId, const std::string& playerName, const ACGame::CharacterAppearance& appearance);
		void UpdatePlayerPositions(const SV_PlayerPositionsPacket& packet);

		ACGame::GameData& GetGameData();
		const ACGame::GameData& GetGameData() const;
		
		void HandleTick(); // TODO
		bool IsRunning() const;

		CL_NetworkedEntitiesManager& GetNetworkedEntitiesManager();
		
		void SetGameState(const SV_GameStatePacket& gameState);
		
		const ACGame::GridData& GetGridData() const;
		void SetGridData(const ACGame::GridData& newGridData);

		void SpawnClientFarmPlot(NetworkEntityId id, Neela::Vector2f position,
			ACGame::FarmPlotState state, ACGame::FarmPlantType farmPlantType);
		void SpawnClientGenericInteractable(NetworkEntityId networkEntityId, const Neela::Vector2f& position, ACGame::GenericInteractableEntitiesType type);
		void OnEntityDestroyReceived(NetworkEntityId id);

	private:
		void Tick();
		void GameTick();

		//void CreateLocalPlayerCharacter();
		void CreateLocalPlayerCharacter(std::uint8_t givenClientId, const ACGame::CharacterAppearance& appearance);
		void CreatePlayerCharacter(std::uint8_t givenClientId, const std::string& playerName, const ACGame::CharacterAppearance& appearance);

		void OnInteractionMouseButtonPressed(bool pressed);
		entt::handle GetEntityAtMousePos(entt::registry& registry, const Neela::Transform& camera);
		void LerpPositions();
		void OffsetPlayerCharacterSprite(entt::handle& playerCharacter, const Neela::Vector2f& offset);
		void ReducePlayerCharactersOffsets();
	};
}
