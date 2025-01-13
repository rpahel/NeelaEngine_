#pragma once 
#include "Common/Clients/sh_Client.h"

#include <entt/entt.hpp>

namespace ACNet
{
	struct CL_Client : public Client
	{
		bool isLocalPlayer;
		entt::handle handle;

		// Prediction
		std::vector<std::pair<IndexInputsPair, Neela::Vector2f>> predictedInputs;
	};

	class CL_ClientsContainer : public ClientsContainer<CL_Client>
	{
	public:
		CL_ClientsContainer() = default;
		~CL_ClientsContainer() = default;
		CL_ClientsContainer(CL_ClientsContainer& other) = delete;
		CL_ClientsContainer(CL_ClientsContainer&& other) = delete;
		CL_ClientsContainer& operator=(CL_ClientsContainer& other) = delete;
		CL_ClientsContainer& operator=(CL_ClientsContainer&& other) = delete;

		CL_Client& AddClient(std::string username, bool isLocalPlayer);
		CL_Client& AddClient(std::uint8_t clientId, std::string username, bool isLocalPlayer);
		void RemoveClient(std::uint8_t clientId);

		const std::vector<CL_Client>& GetClients() const;
		std::vector<CL_Client>& GetMutableClients();

		const CL_Client* GetClient(std::uint8_t clientId) const;
		CL_Client* GetClient(std::uint8_t clientId);

		const CL_Client* GetLocalClient() const;
		CL_Client* GetLocalClient();
	};
}
