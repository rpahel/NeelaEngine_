#include "Common/Network/cl_MessageHandler.h" 
#include "Common/Network/sh_NetworkSerializer.h"
#include "Common/Clients/cl_Client.h"

#include <iostream>
#include <stdexcept>

#include "Common/cl_GameClient.h"
#include "Common/sh_logger.h"
#include "Common/Agriculture/cl_FarmPlotHelpers.h"
#include "Neela/Components/TransformComponent.h"

namespace ACNet
{
	CL_MessageHandler::CL_MessageHandler(GameClient& gameClient, CL_ClientsContainer& clientsContainer)
		: m_serverPeer(nullptr), m_gameClient(gameClient), m_clientsContainer(clientsContainer)
	{
	}

	void CL_MessageHandler::SetServerPeer(ENetPeer* serverPeer)
	{
		m_serverPeer = serverPeer;
	}

	void CL_MessageHandler::HandleReceivedMessage(const ByteArray& byteArray)
	{
		std::size_t offset = 0;
		SV_OpCode opCode = static_cast<SV_OpCode>(NetworkSerializer::Deserialize_u8(byteArray, offset));
		switch (opCode)
		{
			case SV_OpCode::SV_PlayerJoinGame:
			{
				SV_PlayerJoinGamePacket packet;
				packet.NetDeserialize(byteArray, offset);
				ACLog(LogSource::Client, LogType::InfoNetwork, "Player '{0}' joined the game !\n", +packet.attachedClientId);

				if(m_clientsContainer.GetLocalClient()->clientId == 0)
					m_gameClient.OnLocalPlayerGameJoined(packet.attachedClientId, packet.appearance);
				else
				{
					assert(packet.userName.has_value());
					m_gameClient.OnPlayerGameJoined(packet.attachedClientId, packet.userName.value(), packet.appearance);
				}

				break;
			}
			case SV_OpCode::SV_ExistingPlayers:
			{
				SV_ExistingPlayersPacket packet;
				packet.NetDeserialize(byteArray, offset);
				ACLog(LogSource::Client, LogType::InfoNetwork, "Received {0} players' data !\n", packet.players.size());

				for (auto& player : packet.players)
				{
					m_gameClient.OnPlayerGameJoined(player.clientId, player.userName, player.appearance);
				}

				break;
			}
			case SV_OpCode::SV_PlayerPositionsUpdate:
			{
				SV_PlayerPositionsPacket packet;
				packet.NetDeserialize(byteArray, offset);
				//ACLog(LogSource::Client, LogType::InfoNetwork, "Received {0} players' positions !\n", packet.players.size());
				m_gameClient.UpdatePlayerPositions(packet);
				break;
			}
			case SV_OpCode::SV_Chat:
			{
				SV_ChatPacket args;
				args.NetDeserialize(byteArray, offset);
				ACLog(LogSource::Client, LogType::InfoNetwork, "Received chat: '{0}'\n", args.chat);
				break;
			}
			case SV_OpCode::SV_GameState:
			{
				SV_GameStatePacket packet;
				packet.NetDeserialize(byteArray, offset);
				ACLog(LogSource::Client, LogType::InfoNetwork, "Received GameState (farmPlotCount: {0}, autoFarmPlotCount: {1}, genericInteractablesCount: {2})\n",
					packet.farmPlots.size(), packet.autoFarmPlots.size(), packet.genericInteractables.size());
				m_gameClient.SetGameState(packet);
				break;
			}
			case SV_OpCode::SV_DestroyEntity:
			{
				SV_DestroyEntityPacket packet;
				packet.NetDeserialize(byteArray, offset);
				ACLog(LogSource::Client, LogType::InfoNetwork, "Received DestroyEntityPacket for NetId '{0}')\n", packet.id);
				m_gameClient.OnEntityDestroyReceived(packet.id);
				break;
			}
			case SV_OpCode::SV_MoneyUpdate:
			{
				SV_MoneyUpdatePacket packet;
				packet.NetDeserialize(byteArray, offset);
				ACLog(LogSource::Client, LogType::InfoNetwork, "Received MoneyUpdate packet\n");
				m_gameClient.GetGameData().SetMoney(packet.money);
				break;
			}
			case SV_OpCode::SV_SpawnFarmPlot:
			{
				SV_SpawnFarmPlotPacket packet;
				packet.NetDeserialize(byteArray, offset);
				ACLog(LogSource::Client, LogType::InfoNetwork, "Received FarmPlot '{0}' spawn packet\n", packet.farmPlotState.networkId);

				m_gameClient.SpawnClientFarmPlot(packet.farmPlotState.networkId, packet.farmPlotPosition,
					packet.farmPlotState.farmPlotState, packet.farmPlotState.farmPlantType);
				break;	
			}
			case SV_OpCode::SV_FarmPlotsUpdate:
			{
				SV_FarmPlotsUpdate packet;
				packet.NetDeserialize(byteArray, offset);
				ACLog(LogSource::Client, LogType::InfoNetwork, "Received FarmPlotsUdpate\n");

				for (const FarmPlotStatePacket& farmPlotStatePacket : packet.farmPlotStates)
				{
					if(!m_gameClient.GetNetworkedEntitiesManager().HasNetworkedEntity(farmPlotStatePacket.networkId))
					{
						ACLog(LogSource::Client, LogType::Warning, "Received an update to a FarmPlot with an unknown NetId ({0})", farmPlotStatePacket.networkId);	
						continue;
					}
						
					entt::handle networkedEntity = m_gameClient.GetNetworkedEntitiesManager().GetNetworkedEntity(farmPlotStatePacket.networkId);
					if(ACGame::FarmPlotComponent* farmPlotComponent = networkedEntity.try_get<ACGame::FarmPlotComponent>())
					{
						farmPlotStatePacket.Apply(*farmPlotComponent);
						ACGame::FarmPlotHelpers::UpdateFarmPlotVisual(networkedEntity, *farmPlotComponent);
					}
				}
				break;
			}
			case SV_OpCode::SV_SpawnGenericInteractable:
			{
				SV_SpawnGenericInteractablePacket packet;
				packet.NetDeserialize(byteArray, offset);
				ACLog(LogSource::Client, LogType::InfoNetwork, "Received Spawn GenericInteractable for NetId '{0}'\n", packet.instancePacket.networkId);
				m_gameClient.SpawnClientGenericInteractable(packet.instancePacket.networkId, packet.instancePacket.position, packet.instancePacket.type);
				break;		
			}
			default:
				throw std::runtime_error("OpCode received not handled");
		}
	}
}
