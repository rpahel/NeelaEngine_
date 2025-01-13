#pragma once
#include <Common/Clients/sh_Client.h>
#include <enet6/enet.h>

namespace ACNet
{
	struct SV_Client : public Client
	{
		ENetPeer* clientPeer;

		bool hasJoinedGame;

		float jitterBufferAdvancement;
		std::vector<IndexInputsPair> jitterBuffer;
	};

	class SV_ClientsContainer : public ClientsContainer<SV_Client>
	{
	public:
		SV_ClientsContainer() = default;
		~SV_ClientsContainer() = default;
		SV_ClientsContainer(SV_ClientsContainer& other) = delete;
		SV_ClientsContainer(SV_ClientsContainer&& other) = delete;
		SV_ClientsContainer& operator=(SV_ClientsContainer& other) = delete;
		SV_ClientsContainer& operator=(SV_ClientsContainer&& other) = delete;

		SV_Client& CreateNewClient(ENetPeer* clientPeer);
		void RemoveClient(ENetPeer* clientPeer);
		void RemoveClient(std::uint8_t clientId);

		const std::vector<SV_Client>& GetClients() const;
		std::vector<SV_Client>& GetMutableClients();
		std::uint8_t GetInGameClientsCount() const;
		
		const SV_Client* GetClient(std::uint8_t clientId) const;
		SV_Client* GetClient(std::uint8_t clientId);

		const SV_Client* GetClient(ENetPeer* clientPeer) const;
		SV_Client* GetClient(ENetPeer* clientPeer);

	private:
		std::uint8_t GenerateClientId();
	};
}
