#pragma once

#include "sh_Protocols.h"
#include "Common/sh_Constants.h"

#include <vector>
#include <stdexcept>
#include <enet6/enet.h>

namespace ACNet
{
	class GameClient;
	class CL_ClientsContainer;

	class CL_MessageHandler
	{
	private:
		ENetPeer* m_serverPeer;
		GameClient& m_gameClient;
		CL_ClientsContainer& m_clientsContainer;

	public:
		CL_MessageHandler(GameClient& gameClient, CL_ClientsContainer& clientsContainer);

		void SetServerPeer(ENetPeer* serverPeer);

		void HandleReceivedMessage(const ByteArray& byteArray);

		template<class T>
		void SendMessageToServer(const T& packet, std::uint32_t flags);
	};

	template <class T>
	void CL_MessageHandler::SendMessageToServer(const T& packet, std::uint32_t flags)
	{
		if (!m_serverPeer)
			throw std::runtime_error("[CLIENT] Server peer is null while trying to send a message");

		ENetPacket* enetPacket = build_packet(packet, flags);
		enet_peer_send(m_serverPeer, 0, enetPacket);
	}
}
