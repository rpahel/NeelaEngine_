#pragma once

#include "Common/Network/sh_NetworkSerializer.h"

#include <vector>
#include <string>
#include <enet6/enet.h>

#include "sh_NetworkComponent.h"
#include "Common/Agriculture/sh_FarmPlotComponent.h"
#include "Common/Inputs/sh_PlayerInputs.h"
#include "Common/Grid/sh_GridData.h"
#include "Common/Interaction/sh_GenericInteractable.h"
#include "Common/Character/sh_CharacterAppearance.h"
#include "Neela/Maths/Vector.h"

namespace ACNet
{
	using ByteArray = std::vector<std::uint8_t>;

	enum class CL_OpCode : std::uint8_t
	{
		CL_None = 0, // SHOULD NOT BE USED

		// Connection
		CL_JoinGameRequest = 1, // New player info. Sent when trying to join the game.

		// Others
		CL_Chat = 10, // Chat message info. Sent when trying to send a chat message to other players.

		// Player
		CL_PlayerInputs = 20, // Inputs of the player. Sent every time the client consumes its inputs in the game tick.
	};

	enum class SV_OpCode : std::uint8_t
	{
		SV_None = 0, // SHOULD NOT BE USED

		// Connection
		SV_PlayerJoinGame = 1, // New player info (cliendId, ...). Sent to the client requesting to join the game.
		SV_NewPlayerJoined = 2, // New player info. Sent to all existing players in the game when a new player joins.
		SV_ExistingPlayers = 3, // Data of all players currently connected. Sent to new player and it joins.

		// Others
		SV_Chat = 10, // Chat message sent by a player. Sent to all players when receiving a chat.

		// Players
		SV_PlayerPositionsUpdate = 20, // Positions of all players. Sent regularly to all clients.

		// Game
		SV_GameState = 40, // Game state data. Sent to a player which just joined. TODO: define this more precisely
		SV_DestroyEntity = 41, // Network id of the entity to destroy. Sent when destroying an entity on server side.
		SV_MoneyUpdate = 42, // New money count. Sent in the network tick when the amount changed
		SV_SpawnFarmPlot = 43, // Farm plot data. Sent to all players when a farm plot is spawned on server side.
		SV_FarmPlotsUpdate = 44, // State of all farm plot data which changed. Sent to all players during network tick.
		SV_SpawnGenericInteractable = 45, // Data for constructible interactable. Sent to all players when a generic interactable is spawned on server side.
	};

	//====================================================================================
	//==== CONNECTION
	//====================================================================================

	struct CL_PlayerJoinGameRequestPacket : INetworkSerializeable
	{
		static constexpr CL_OpCode OpCode = CL_OpCode::CL_JoinGameRequest;

		std::string username;
		ACGame::CharacterAppearance appearance;

		void NetSerialize(ByteArray& byteArray) const override;
		void NetDeserialize(const ByteArray& byteArray, std::size_t& offset) override;
	};

	struct SV_PlayerJoinGamePacket : INetworkSerializeable
	{
		static constexpr SV_OpCode OpCode = SV_OpCode::SV_PlayerJoinGame;

		std::uint8_t attachedClientId;
		ACGame::CharacterAppearance appearance;
		std::optional<std::string> userName;

		void NetSerialize(ByteArray& byteArray) const override;
		void NetDeserialize(const ByteArray& byteArray, std::size_t& offset) override;
	};

	//====================================================================================
	//==== OTHERS
	//====================================================================================

	struct CL_ChatPacket : INetworkSerializeable
	{
	public:
		static constexpr CL_OpCode OpCode = CL_OpCode::CL_Chat;

		std::string chat;
		std::uint8_t senderClientId;

	public:
		CL_ChatPacket() = default;
		CL_ChatPacket(std::string chat, std::uint8_t playerId);

		void NetSerialize(ByteArray& byteArray) const override;
		void NetDeserialize(const ByteArray& byteArray, std::size_t& offset) override;
	};

	struct SV_ChatPacket : INetworkSerializeable
	{
	public:
		static constexpr SV_OpCode OpCode = SV_OpCode::SV_Chat;

		std::string chat;
		std::uint8_t senderClientId;

	public:
		SV_ChatPacket() = default;
		SV_ChatPacket(std::string chat, std::uint8_t playerId);

		void NetSerialize(ByteArray& byteArray) const override;
		void NetDeserialize(const ByteArray& byteArray, std::size_t& offset) override;
	};

	//====================================================================================
	//==== PLAYERS
	//====================================================================================

	struct CL_PlayerInputsPacket : INetworkSerializeable
	{
	public:
		static constexpr CL_OpCode OpCode = CL_OpCode::CL_PlayerInputs;

		ACGame::PlayerInputs inputs;

		CL_PlayerInputsPacket() = default;
		void NetSerialize(ByteArray& byteArray) const override;
		void NetDeserialize(const ByteArray& byteArray, std::size_t& offset) override;
	};

	struct SV_ExistingPlayersPacket : INetworkSerializeable
	{
	public:
		static constexpr SV_OpCode OpCode = SV_OpCode::SV_ExistingPlayers;

		struct PlayerData
		{
			std::uint8_t clientId;
			std::string userName;
			ACGame::CharacterAppearance appearance;
		};

		std::vector<PlayerData> players;
		
		SV_ExistingPlayersPacket() = default;
		void NetSerialize(ByteArray& byteArray) const override;
		void NetDeserialize(const ByteArray& byteArray, std::size_t& offset) override;
	};

	struct PlayerData
	{
		std::uint8_t clientId;
		Neela::Vector2f position;
	};

	struct SV_PlayerPositionsPacket : INetworkSerializeable
	{
	public:
		static constexpr SV_OpCode OpCode = SV_OpCode::SV_PlayerPositionsUpdate;

		std::uint32_t lastInputIndex;
		std::uint32_t tickIndex;
		std::vector<PlayerData> players;

		SV_PlayerPositionsPacket() = default;
		void NetSerialize(ByteArray& byteArray) const override;
		void NetDeserialize(const ByteArray& byteArray, std::size_t& offset) override;
	};

	//====================================================================================
	//==== GAME
	//====================================================================================

	struct FarmPlotStatePacket : INetworkSerializeable
	{
		FarmPlotStatePacket() = default;
		FarmPlotStatePacket(NetworkEntityId networkId, const ACGame::FarmPlotComponent& farmPlotComponent);
		FarmPlotStatePacket(NetworkEntityId networkId, ACGame::FarmPlotState state, ACGame::FarmPlantType plantType);
		
		NetworkEntityId networkId;
		ACGame::FarmPlotState farmPlotState;
		ACGame::FarmPlantType farmPlantType;

		void Apply(ACGame::FarmPlotComponent& farmPlotComponent) const;
		
		void NetSerialize(ByteArray& byteArray) const override;
		void NetDeserialize(const ByteArray& byteArray, std::size_t& offset) override;
	};

	struct FarmPlotInstancePacket : INetworkSerializeable
	{
		FarmPlotInstancePacket() = default;
		FarmPlotInstancePacket(const FarmPlotStatePacket& farmPlotStatePacket, const Neela::Vector2f& position);

		FarmPlotStatePacket farmPlotStatePacket;
		Neela::Vector2f position;
		
		void NetSerialize(::ByteArray& byteArray) const override;
		void NetDeserialize(const ::ByteArray& byteArray, std::size_t& offset) override;
	};

	struct GenericInteractableInstancePacket : INetworkSerializeable
	{
		GenericInteractableInstancePacket() = default;
		GenericInteractableInstancePacket(NetworkEntityId networkId, ACGame::GenericInteractableEntitiesType type, const Neela::Vector2f& position);

		NetworkEntityId networkId;
		ACGame::GenericInteractableEntitiesType type;
		Neela::Vector2f position;

		void NetSerialize(ByteArray& byteArray) const override;
		void NetDeserialize(const ByteArray& byteArray, std::size_t& offset) override;
	};
	
	struct SV_GameStatePacket : INetworkSerializeable
	{
		static constexpr SV_OpCode OpCode = SV_OpCode::SV_GameState;

		ACGame::GridData gridData;
		std::uint32_t money;
		std::vector<FarmPlotInstancePacket> farmPlots;
		std::vector<FarmPlotInstancePacket> autoFarmPlots;
		std::vector<GenericInteractableInstancePacket> genericInteractables;

		void NetSerialize(ByteArray& byteArray) const override;
		void NetDeserialize(const ByteArray& byteArray, std::size_t& offset) override;
	};

	struct SV_DestroyEntityPacket : INetworkSerializeable
	{
		static constexpr SV_OpCode OpCode = SV_OpCode::SV_DestroyEntity;

		NetworkEntityId id;

		void NetSerialize(ByteArray& byteArray) const;
		void NetDeserialize(const ::ByteArray& byteArray, std::size_t& offset) override;
	};

	struct SV_MoneyUpdatePacket : INetworkSerializeable
	{
		static constexpr SV_OpCode OpCode = SV_OpCode::SV_MoneyUpdate;

		std::uint32_t money;
		
		void NetSerialize(::ByteArray& byteArray) const override;
		void NetDeserialize(const ::ByteArray& byteArray, std::size_t& offset) override;
	};
	
	struct SV_SpawnFarmPlotPacket : INetworkSerializeable
	{
		static constexpr SV_OpCode OpCode = SV_OpCode::SV_SpawnFarmPlot;

		bool automatedFarmPlot = false;
		FarmPlotStatePacket farmPlotState;
		Neela::Vector2f farmPlotPosition;

		void NetSerialize(ByteArray& byteArray) const override;
		void NetDeserialize(const ByteArray& byteArray, std::size_t& offset) override;
	};

	struct SV_FarmPlotsUpdate : INetworkSerializeable
	{
		static constexpr SV_OpCode OpCode = SV_OpCode::SV_FarmPlotsUpdate;

		std::vector<FarmPlotStatePacket> farmPlotStates;

		void NetSerialize(::ByteArray& byteArray) const override;
		void NetDeserialize(const ::ByteArray& byteArray, std::size_t& offset) override;
	};

	struct SV_SpawnGenericInteractablePacket : INetworkSerializeable
	{
		static constexpr SV_OpCode OpCode = SV_OpCode::SV_SpawnGenericInteractable;

		GenericInteractableInstancePacket instancePacket;

		void NetSerialize(ByteArray& byteArray) const override;
		void NetDeserialize(const ByteArray& byteArray, std::size_t& offset) override;
	};

	//====================================================================================
	//==== BUILD PACKET
	//====================================================================================

	template<typename T>
	ENetPacket* build_packet(const T& packet, enet_uint32 flags)
	{
		ByteArray byteArray;

		NetworkSerializer::Serialize_u8(byteArray, static_cast<std::uint8_t>(T::OpCode));
		packet.NetSerialize(byteArray);

		return enet_packet_create(byteArray.data(), byteArray.size(), flags);
	}
}
