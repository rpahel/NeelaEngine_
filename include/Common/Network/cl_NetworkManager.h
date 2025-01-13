#pragma once

#include "Common/Network/cl_MessageHandler.h"

#include <string>
#include <enet6/enet.h>

namespace ACNet
{
	class GameClient;
	class CL_ClientsContainer;

	class CL_NetworkManager
	{
	private:
		ENetHost* m_host;
		ENetPeer* m_serverPeer;

		bool m_failedToConnect = false;

		GameClient& m_gameClient;
		CL_MessageHandler& m_messageHandler;
		CL_ClientsContainer& m_clientsContainer;

	public:
		CL_NetworkManager(GameClient& gameClient, CL_MessageHandler& messageHandler, CL_ClientsContainer& clientsContainer);
		~CL_NetworkManager();
		CL_NetworkManager(CL_NetworkManager& other) = delete;
		CL_NetworkManager(CL_NetworkManager&& other) = delete;
		CL_NetworkManager& operator=(CL_NetworkManager& other) = delete;
		CL_NetworkManager& operator=(CL_NetworkManager&& other) = delete;

		bool SendServerConnectionRequest(std::string targetIpAddress);
		bool IsConnectedToServer() const;
		bool IsConnectionInProgress() const;
		bool DoesConnectionFailed() const;

		void HandleNetworkEvents();

		ENetHost* GetHost();
		ENetPeer* GetServerPeer();
	};
}