#include "Common/Network/sh_protocols.h"

namespace ACNet
{
	//====================================================================================
	//==== CONNECTION
	//====================================================================================

	void CL_PlayerJoinGameRequestPacket::NetSerialize(ByteArray& byteArray) const
	{
		NetworkSerializer::Serialize_str(byteArray, username);
		appearance.NetSerialize(byteArray);
	}

	void CL_PlayerJoinGameRequestPacket::NetDeserialize(const ByteArray& byteArray, std::size_t& offset)
	{
		username = NetworkSerializer::Deserialize_str(byteArray, offset);
		appearance.NetDeserialize(byteArray, offset);
	}

	void SV_PlayerJoinGamePacket::NetSerialize(ByteArray& byteArray) const
	{
		NetworkSerializer::Serialize_u8(byteArray, attachedClientId);
		appearance.NetSerialize(byteArray);

		NetworkSerializer::Serialize_u8(byteArray, userName.has_value());
		if (userName.has_value())
			NetworkSerializer::Serialize_str(byteArray, userName.value());
	}

	void SV_PlayerJoinGamePacket::NetDeserialize(const ByteArray& byteArray, std::size_t& offset)
	{
		attachedClientId = NetworkSerializer::Deserialize_u8(byteArray, offset);
		appearance.NetDeserialize(byteArray, offset);

		if (NetworkSerializer::Deserialize_u8(byteArray, offset) /*hasUserName*/)
			userName = NetworkSerializer::Deserialize_str(byteArray, offset);
	}

	//====================================================================================
	//==== OTHERS
	//====================================================================================

	CL_ChatPacket::CL_ChatPacket(std::string chat, std::uint8_t playerId) :
		chat(std::move(chat)),
		senderClientId(playerId)
	{
	}

	void CL_ChatPacket::NetSerialize(ByteArray& byteArray) const
	{
		NetworkSerializer::Serialize_u8(byteArray, senderClientId);
		NetworkSerializer::Serialize_str(byteArray, chat);
	}

	void CL_ChatPacket::NetDeserialize(const ByteArray& byteArray, std::size_t& offset)
	{
		senderClientId = NetworkSerializer::Deserialize_u8(byteArray, offset);
		chat = NetworkSerializer::Deserialize_str(byteArray, offset);
	}

	SV_ChatPacket::SV_ChatPacket(std::string chat, std::uint8_t playerId) :
		chat(std::move(chat)),
		senderClientId(playerId)
	{
	}

	void SV_ChatPacket::NetSerialize(ByteArray& byteArray) const
	{
		NetworkSerializer::Serialize_u8(byteArray, senderClientId);
		NetworkSerializer::Serialize_str(byteArray, chat);
	}

	void SV_ChatPacket::NetDeserialize(const ByteArray& byteArray, std::size_t& offset)
	{
		senderClientId = NetworkSerializer::Deserialize_u8(byteArray, offset);
		chat = NetworkSerializer::Deserialize_str(byteArray, offset);
	}

	//====================================================================================
	//==== PLAYERS
	//====================================================================================

	void CL_PlayerInputsPacket::NetSerialize(ByteArray& byteArray) const
	{
		inputs.NetSerialize(byteArray);
	}

	void CL_PlayerInputsPacket::NetDeserialize(const ByteArray& byteArray, std::size_t& offset)
	{
		inputs.NetDeserialize(byteArray, offset);
	}

	void SV_ExistingPlayersPacket::NetSerialize(ByteArray& byteArray) const
	{
		NetworkSerializer::Serialize_u16(byteArray, players.size());
		for (const auto& playerData : players)
		{
			NetworkSerializer::Serialize_u8(byteArray, playerData.clientId);
			NetworkSerializer::Serialize_str(byteArray, playerData.userName);
			playerData.appearance.NetSerialize(byteArray);
		}
	}

	void SV_ExistingPlayersPacket::NetDeserialize(const ByteArray& byteArray, std::size_t& offset)
	{
		players.resize(NetworkSerializer::Deserialize_u16(byteArray, offset));
		for (auto& playerData : players)
		{
			playerData.clientId = NetworkSerializer::Deserialize_u8(byteArray, offset);
			playerData.userName = NetworkSerializer::Deserialize_str(byteArray, offset);
			playerData.appearance.NetDeserialize(byteArray, offset);
		}
	}

	void SV_PlayerPositionsPacket::NetSerialize(ByteArray& byteArray) const
	{
		NetworkSerializer::Serialize_u32(byteArray, lastInputIndex);
		NetworkSerializer::Serialize_u32(byteArray, tickIndex);

		NetworkSerializer::Serialize_u16(byteArray, players.size());
		for (const auto& playerData : players)
		{
			NetworkSerializer::Serialize_u8(byteArray, playerData.clientId);
			NetworkSerializer::Serialize_f(byteArray, playerData.position.X);
			NetworkSerializer::Serialize_f(byteArray, playerData.position.Y);
		}
	}

	void SV_PlayerPositionsPacket::NetDeserialize(const ByteArray& byteArray, std::size_t& offset)
	{
		lastInputIndex = NetworkSerializer::Deserialize_u32(byteArray, offset);
		tickIndex = NetworkSerializer::Deserialize_u32(byteArray, offset);

		players.resize(NetworkSerializer::Deserialize_u16(byteArray, offset));
		for (auto& playerData : players)
		{
			playerData.clientId = NetworkSerializer::Deserialize_u8(byteArray, offset);
			playerData.position.X = NetworkSerializer::Deserialize_f(byteArray, offset);
			playerData.position.Y = NetworkSerializer::Deserialize_f(byteArray, offset);
		}
	}

	//====================================================================================
	//==== GAME
	//====================================================================================

	FarmPlotStatePacket::FarmPlotStatePacket(NetworkEntityId networkId, const ACGame::FarmPlotComponent& farmPlotComponent)
	: FarmPlotStatePacket(networkId, farmPlotComponent.state, farmPlotComponent.plantType)
	{
	}

	FarmPlotStatePacket::FarmPlotStatePacket(NetworkEntityId networkId, ACGame::FarmPlotState state,
	                                         ACGame::FarmPlantType plantType) :
	networkId(networkId),
	farmPlotState(state),
	farmPlantType(plantType)
	{
	}

	void FarmPlotStatePacket::Apply(ACGame::FarmPlotComponent& farmPlotComponent) const
	{
		farmPlotComponent.state = farmPlotState;
		farmPlotComponent.plantType = farmPlantType;
	}
	
	void FarmPlotStatePacket::NetSerialize(ByteArray& byteArray) const
	{
		NetworkSerializer::Serialize_NetworkEntityId(byteArray, networkId);
		NetworkSerializer::Serialize_u8(byteArray, static_cast<std::uint8_t>(farmPlotState));
		NetworkSerializer::Serialize_u8(byteArray, static_cast<std::uint8_t>(farmPlantType));
	}

	void FarmPlotStatePacket::NetDeserialize(const ByteArray& byteArray, std::size_t& offset)
	{
		networkId = NetworkSerializer::Deserialize_NetworkEntityId(byteArray, offset);
		farmPlotState = static_cast<ACGame::FarmPlotState>(NetworkSerializer::Deserialize_u8(byteArray, offset));
		farmPlantType = static_cast<ACGame::FarmPlantType>(NetworkSerializer::Deserialize_u8(byteArray, offset));
	}

	FarmPlotInstancePacket::FarmPlotInstancePacket(const FarmPlotStatePacket& farmPlotStatePacket, const Neela::Vector2f& position) :
	farmPlotStatePacket(farmPlotStatePacket),
	position(position)
	{
	}

	void FarmPlotInstancePacket::NetSerialize(ByteArray& byteArray) const
	{
		farmPlotStatePacket.NetSerialize(byteArray);
		NetworkSerializer::Serialize_Vector2f(byteArray, position);
	}

	void FarmPlotInstancePacket::NetDeserialize(const ByteArray& byteArray, std::size_t& offset)
	{
		farmPlotStatePacket.NetDeserialize(byteArray, offset);
		position = NetworkSerializer::Deserialize_Vector2f(byteArray, offset);
	}

	GenericInteractableInstancePacket::GenericInteractableInstancePacket(NetworkEntityId networkId,
		ACGame::GenericInteractableEntitiesType type,
		const Neela::Vector2f& position) :
	networkId(networkId),
	type(type),
	position(position)
	{
	}

	void GenericInteractableInstancePacket::NetSerialize(ByteArray& byteArray) const
	{
		NetworkSerializer::Serialize_NetworkEntityId(byteArray, networkId);
		NetworkSerializer::Serialize_u8(byteArray, static_cast<std::uint8_t>(type));
		NetworkSerializer::Serialize_Vector2f(byteArray, position);
	}

	void GenericInteractableInstancePacket::NetDeserialize(const ByteArray& byteArray, std::size_t& offset)
	{
		networkId = NetworkSerializer::Deserialize_NetworkEntityId(byteArray, offset);
		type = static_cast<ACGame::GenericInteractableEntitiesType>(NetworkSerializer::Deserialize_u8(byteArray, offset));
		position = NetworkSerializer::Deserialize_Vector2f(byteArray, offset);
	}

	void SV_GameStatePacket::NetSerialize(ByteArray& byteArray) const
	{
		gridData.NetSerialize(byteArray);
		NetworkSerializer::Serialize_u32(byteArray, money);

		NetworkSerializer::Serialize_List(byteArray, farmPlots);
		NetworkSerializer::Serialize_List(byteArray, autoFarmPlots);

		NetworkSerializer::Serialize_List(byteArray, genericInteractables);
	}

	void SV_GameStatePacket::NetDeserialize(const ByteArray& byteArray, std::size_t& offset)
	{
		gridData.NetDeserialize(byteArray, offset);
		money = NetworkSerializer::Deserialize_u32(byteArray, offset);
		
		farmPlots = NetworkSerializer::Deserialize_List<FarmPlotInstancePacket>(byteArray, offset);
		autoFarmPlots = NetworkSerializer::Deserialize_List<FarmPlotInstancePacket>(byteArray, offset);

		genericInteractables = NetworkSerializer::Deserialize_List<GenericInteractableInstancePacket>(byteArray, offset);
	}

	void SV_DestroyEntityPacket::NetSerialize(ByteArray& byteArray) const
	{
		NetworkSerializer::Serialize_NetworkEntityId(byteArray, id);
	}

	void SV_DestroyEntityPacket::NetDeserialize(const ByteArray& byteArray, std::size_t& offset)
	{
		id = NetworkSerializer::Deserialize_NetworkEntityId(byteArray, offset);
	}

	void SV_MoneyUpdatePacket::NetSerialize(ByteArray& byteArray) const
	{
		NetworkSerializer::Serialize_u32(byteArray, money);
	}

	void SV_MoneyUpdatePacket::NetDeserialize(const ByteArray& byteArray, std::size_t& offset)
	{
		money = NetworkSerializer::Deserialize_u32(byteArray, offset);
	}

	void SV_SpawnFarmPlotPacket::NetSerialize(ByteArray& byteArray) const
	{
		NetworkSerializer::Serialize_u8(byteArray, automatedFarmPlot);
		farmPlotState.NetSerialize(byteArray);
		NetworkSerializer::Serialize_f(byteArray, farmPlotPosition.X);
		NetworkSerializer::Serialize_f(byteArray, farmPlotPosition.Y);
	}

	void SV_SpawnFarmPlotPacket::NetDeserialize(const ByteArray& byteArray, std::size_t& offset)
	{
		automatedFarmPlot = NetworkSerializer::Deserialize_u8(byteArray, offset);
		farmPlotState.NetDeserialize(byteArray, offset);
		farmPlotPosition.X = NetworkSerializer::Deserialize_f(byteArray, offset);
		farmPlotPosition.Y = NetworkSerializer::Deserialize_f(byteArray, offset);
	}

	void SV_FarmPlotsUpdate::NetSerialize(ByteArray& byteArray) const
	{
		NetworkSerializer::Serialize_List(byteArray, farmPlotStates);
	}

	void SV_FarmPlotsUpdate::NetDeserialize(const ByteArray& byteArray, std::size_t& offset)
	{
		farmPlotStates = NetworkSerializer::Deserialize_List<FarmPlotStatePacket>(byteArray, offset);
	}

	void SV_SpawnGenericInteractablePacket::NetSerialize(ByteArray& byteArray) const
	{
		instancePacket.NetSerialize(byteArray);
	}

	void SV_SpawnGenericInteractablePacket::NetDeserialize(const ByteArray& byteArray, std::size_t& offset)
	{
		instancePacket.NetDeserialize(byteArray, offset);
	}
}
