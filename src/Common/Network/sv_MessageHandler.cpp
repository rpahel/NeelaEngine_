#include "Common/Network/sv_MessageHandler.h"

#include <cassert>

#include "Common/sv_GameServer.h"
#include "Common/Clients/sv_Client.h"

namespace ACNet
{
	SV_MessageHandler::SV_MessageHandler(GameServer& gameServer, SV_ClientsContainer& clientsContainer)
		: m_gameServer(gameServer),
		m_clientsContainer(clientsContainer)
	{
	}

	void SV_MessageHandler::HandleReceivedMessage(const ByteArray& byteArray, ENetPeer* sender)
	{
		std::size_t offset = 0;
		CL_OpCode opCode = static_cast<CL_OpCode>(NetworkSerializer::Deserialize_u8(byteArray, offset));
		switch (opCode)
		{
			case CL_OpCode::CL_JoinGameRequest:
			{
				CL_PlayerJoinGameRequestPacket packet;
				packet.NetDeserialize(byteArray, offset);
				ACLog(LogSource::Server, LogType::InfoNetwork, "Received join game request as '{}'\n", packet.username);
				SV_Client* client = m_clientsContainer.GetClient(sender);
				assert(client);
				client->username = packet.username;
				client->appearance = packet.appearance;
				client->hasJoinedGame = true;

				m_gameServer.OnNewPlayerJoined(client);
				break;
			}
			case CL_OpCode::CL_PlayerInputs:
			{
				CL_PlayerInputsPacket packet;
				packet.NetDeserialize(byteArray, offset);
				//ACLog(LogSource::Server, LogType::InfoNetwork, "Received player inputs.\n");

				SV_Client* client = m_clientsContainer.GetClient(sender);
				assert(client);

				client->jitterBuffer.push_back({ client->lastInputIndex, packet.inputs });
				client->lastInputIndex++;
				break;
			}
			case CL_OpCode::CL_Chat:
			{
				CL_ChatPacket args;
				args.NetDeserialize(byteArray, offset);
				ACLog(LogSource::Server, LogType::InfoNetwork, "Received chat : {}\n", args.chat);
				// TODO: do something
				break;
			}
			default:
			{
				ACLogError(LogSource::Server, "CL_OpCode received not handled.\n");
				throw std::runtime_error("OpCode received not handled."); // Vraiment necessaire ? // go crash l'appli mwhahaha
				break;
			}
		}
	}
}
