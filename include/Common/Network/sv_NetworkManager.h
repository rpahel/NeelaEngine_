#pragma once

#include <enet6/enet.h>

namespace ACNet
{
	class SV_MessageHandler;
	class GameServer;

	class SV_NetworkManager
	{
	private:
		ENetHost* m_host;
		GameServer& m_gameServer;
		SV_MessageHandler& m_messageHandler;
		SV_ClientsContainer& m_clientsContainer;

	public:
		SV_NetworkManager(GameServer& gameServer, SV_MessageHandler& messageHandler, SV_ClientsContainer& clientsContainer);
		~SV_NetworkManager();
		SV_NetworkManager(SV_NetworkManager& other) = delete;
		SV_NetworkManager(SV_NetworkManager&& other) = delete;
		SV_NetworkManager& operator=(SV_NetworkManager& other) = delete;
		SV_NetworkManager& operator=(SV_NetworkManager&& other) = delete;

		void StartServer();
		bool IsServerStarted() const;

		void HandleNetworkEvents();

		ENetHost* GetNetHost();
	};
}
