#include "Common/Network/cl_NetworkManager.h" 
#include "Common/Clients/cl_Client.h"
#include "Common/sh_constants.h"

#include <vector>
#include <cassert>
#include <cstdint>
#include <fmt/core.h>
#include <fmt/color.h>

#include "Common/sh_logger.h"

namespace ACNet
{
	CL_NetworkManager::CL_NetworkManager(GameClient& gameClient, CL_MessageHandler& messageHandler,
		CL_ClientsContainer& clientsContainer) :
		m_host(nullptr),
		m_serverPeer(nullptr),
		m_gameClient(gameClient),
		m_messageHandler(messageHandler),
		m_clientsContainer(clientsContainer)
	{
	}

	CL_NetworkManager::~CL_NetworkManager()
	{
		if (m_serverPeer)
			enet_peer_disconnect_now(m_serverPeer, 0);
		m_serverPeer = nullptr;

		if (m_host)
		{
			enet_host_flush(m_host);
			enet_host_destroy(m_host);
		}
		m_host = nullptr;
	}

	bool CL_NetworkManager::SendServerConnectionRequest(std::string targetIpAddress)
	{
		ACLog(LogSource::Client, LogType::InfoNetwork, "Connecting to {}.\n", targetIpAddress);

		ENetAddress serverAddress;
		if (enet_address_set_host(&serverAddress, ENET_ADDRESS_TYPE_ANY, targetIpAddress.data()) != 0)
		{
			ACLogError(LogSource::Client, "Failed to resolve address.\n");
			return false;
		}
		serverAddress.port = AppPort;

		if (m_host)
			enet_host_destroy(m_host);

		m_host = enet_host_create(serverAddress.type, nullptr, 1, 0, 0, 0);
		if (!m_host)
		{
			ACLogError(LogSource::Client, "Failed to initialize host.\n");
			return false;
		}

		m_failedToConnect = false;
		m_serverPeer = enet_host_connect(m_host, &serverAddress, 0, 0); // connection request sent
		if(!m_serverPeer)
			return false;
		return true;
	}

	bool CL_NetworkManager::IsConnectedToServer() const
	{
		return m_serverPeer && m_serverPeer->state == ENET_PEER_STATE_CONNECTED;
	}

	bool CL_NetworkManager::IsConnectionInProgress() const
	{
		return m_serverPeer;
	}

	bool CL_NetworkManager::DoesConnectionFailed() const
	{
		return m_failedToConnect;
	}

	void CL_NetworkManager::HandleNetworkEvents()
	{
		ENetEvent event;
		if (enet_host_service(m_host, &event, 0) > 0)
		{
			do
			{
				switch (event.type)
				{
					case ENET_EVENT_TYPE_NONE:
						{
							if (m_serverPeer->state != ENET_PEER_STATE_CONNECTED)
							{
								enet_peer_reset(m_serverPeer);
								m_serverPeer = nullptr;
								m_failedToConnect = true;
								ACLogError(LogSource::Client, "Failed to connect.\n");
							}
							else
							{
								ACLogError(LogSource::Client, "Received unexpected network event.\n");
							}
							break;
						}
					case ENET_EVENT_TYPE_CONNECT:
						{
							ACLog(LogSource::Client, LogType::InfoNetwork, "Successfully connected to the server.\n");
							m_messageHandler.SetServerPeer(m_serverPeer);
							break;
						}
					case ENET_EVENT_TYPE_DISCONNECT:
						ACLog(LogSource::Client, LogType::InfoNetwork, "Received disconnected from server event.\n");
						// TODO: call GameClient function
						return;

					case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
						ACLog(LogSource::Client, LogType::InfoNetwork, "Received timed out event.\n");
						m_failedToConnect = true;
						// TODO: call GameClient function
						return;

					case ENET_EVENT_TYPE_RECEIVE:
					{
						std::vector<std::uint8_t> content(event.packet->dataLength);
						std::memcpy(content.data(), event.packet->data, event.packet->dataLength);

						m_messageHandler.HandleReceivedMessage(content);

						enet_packet_destroy(event.packet);
						break;
					}
				}
			} while (enet_host_check_events(m_host, &event) > 0);
		}
	}

	ENetHost* CL_NetworkManager::GetHost()
	{
		return m_host;
	}

	ENetPeer* CL_NetworkManager::GetServerPeer()
	{
		return m_serverPeer;
	}
}
