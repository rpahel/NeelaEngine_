#pragma once
#include "sh_Protocols.h"
#include <fmt/color.h>

#include "Common/sh_logger.h"
#include "Common/Clients/sv_Client.h"

namespace ACNet
{
	class GameServer;

	class SV_MessageHandler
	{
	private:
		GameServer& m_gameServer;
		SV_ClientsContainer& m_clientsContainer;

	public:
		SV_MessageHandler(GameServer& gameServer, SV_ClientsContainer& clientsContainer);
		~SV_MessageHandler() = default;
		SV_MessageHandler(SV_MessageHandler& other) = delete;
		SV_MessageHandler(SV_MessageHandler&& other) = delete;
		SV_MessageHandler& operator=(SV_MessageHandler& other) = delete;
		SV_MessageHandler& operator=(SV_MessageHandler&& other) = delete;

		void HandleReceivedMessage(const ByteArray& byteArray, ENetPeer* sender);

		// TODO: do more send message functions
		template<class T>
		void SendMessageToClient(ENetPeer* clientPeer, const T& packet, std::uint32_t flags);

		template<class T>
		void SendMessageToClient(const SV_Client* client, const T& packet, std::uint32_t flags);

		template<class T>
		void SendMessageToClients(const T& packet, std::uint32_t flags, const SV_Client* ignore = nullptr);
	};

	template <class T>
	void SV_MessageHandler::SendMessageToClient(ENetPeer* clientPeer, const T& packet, std::uint32_t flags)
	{
		if (!clientPeer)
		{
			ACLogError(LogSource::Server, "Trying to send message to a null Peer (SendMessageToClient).\n");
			return;
		}

		ENetPacket* enetPacket = build_packet(packet, flags);
		enet_peer_send(clientPeer, 0, enetPacket);
	}

	template <class T>
	void SV_MessageHandler::SendMessageToClient(const SV_Client* client, const T& packet, std::uint32_t flags)
	{
		if (!client)
		{
			ACLogError(LogSource::Server, "Trying to send message to a null client.\n");
			return;
		}
		
		SendMessageToClient(client->clientPeer, packet, flags);
	}

	template <class T>
	void SV_MessageHandler::SendMessageToClients(const T& packet, std::uint32_t flags, const SV_Client* ignore)
	{
		ENetPacket* enetPacket = nullptr;
		
		for (const SV_Client& client : m_clientsContainer.GetClients())
		{
			if(!client.hasJoinedGame) // Ignore clients which did not joined the game yet
				continue;
			
			if(ignore && &client == ignore)
				continue;

			if(!client.clientPeer)
			{
				ACLogError(LogSource::Server, "Trying to send message to a null Peer (SendMessageToClient**s**).\n");
				return;
			}

			if(!enetPacket)
				enetPacket = build_packet(packet, flags);
			enet_peer_send(client.clientPeer, 0, enetPacket);
		}
	}
}
