#include <Common/sh_Constants.h>
#include <Common/Network/sv_MessageHandler.h>
#include <Common/Network/sv_NetworkManager.h>

#include <iostream>
#include <stdexcept>

#include "Common/Clients/sv_Client.h"


namespace ACNet
{
	SV_NetworkManager::SV_NetworkManager(GameServer& gameServer, SV_MessageHandler& messageHandler,
		SV_ClientsContainer& clientsContainer) :
	m_host(nullptr),
	m_gameServer(gameServer),
	m_messageHandler(messageHandler),
	m_clientsContainer(clientsContainer)
	{
	}

	SV_NetworkManager::~SV_NetworkManager()
	{
		if(m_host)
			enet_host_destroy(m_host);
	}

	void SV_NetworkManager::StartServer()
	{
		ENetAddress address;
		enet_address_build_any(&address, ENET_ADDRESS_TYPE_IPV6);
		address.port = AppPort;

		m_host = enet_host_create(ENET_ADDRESS_TYPE_ANY, &address, 10, 10, 0, 0);
		if (!m_host)
		{
			ACLogError(LogSource::Server, "Failed to create ENet host (is port free?).\n");
			throw std::runtime_error("[Server:] Failed to create ENet host (is port free?).");
		}

		ACLog(LogSource::Server, LogType::InfoGameState, "Server started !\n");
	}

	bool SV_NetworkManager::IsServerStarted() const
	{
		return m_host;
	}

	void SV_NetworkManager::HandleNetworkEvents()
	{
		ENetEvent event;
		while (enet_host_service(m_host, &event, 1) > 0)
		{
			do
			{
				switch (event.type)
				{
				case ENetEventType::ENET_EVENT_TYPE_CONNECT:
						ACLog(LogSource::Server, LogType::InfoNetwork, "Peer #{} connected !\n", enet_peer_get_id(event.peer));
						m_clientsContainer.CreateNewClient(event.peer);
						// TODO: proper connection
						break;

					case ENetEventType::ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
					case ENetEventType::ENET_EVENT_TYPE_DISCONNECT:
						ACLog(LogSource::Server, LogType::InfoNetwork, "Peer #{} disconnected !\n", enet_peer_get_id(event.peer));
						m_clientsContainer.RemoveClient(event.peer);
						// TODO: proper disconnection
						break;

					case ENetEventType::ENET_EVENT_TYPE_RECEIVE:
						{
							// ACLog(LogSource::Server, LogType::InfoNetwork, "Received data from peer#{0} ({1} bytes) !\n", enet_peer_get_id(event.peer), enet_packet_get_length(event.packet));

							std::vector<std::uint8_t> content(event.packet->dataLength);
							std::memcpy(content.data(), event.packet->data, event.packet->dataLength);
							m_messageHandler.HandleReceivedMessage(content, event.peer);

							enet_packet_dispose(event.packet);
							break;
						}

					case ENetEventType::ENET_EVENT_TYPE_NONE:
						ACLog(LogSource::Server, LogType::InfoNetwork, "Event of type NONE detected.\n");
						break;
				}
			} while (enet_host_check_events(m_host, &event) > 0);
		}
	}

	ENetHost* SV_NetworkManager::GetNetHost()
	{
		return m_host;
	}
}
