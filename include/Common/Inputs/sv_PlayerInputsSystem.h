#pragma once
#include <entt/entity/registry.hpp>

#include "Common/sh_GameData.h"
#include "Common/Network/sv_NetworkedEntitiesManager.h"

namespace ACNet
{
	class NetworkedEntitiesManager;
	class SV_ClientsContainer;
	class SV_Client;
}

namespace ACGame
{
	class PlayerInputsSystem
	{
	public:
		static void ProcessPlayerInputs(ACNet::SV_ClientsContainer* clients, ACNet::SV_NetworkedEntitiesManager* entitiesManager, ACNet::SV_MessageHandler*
		                                messageHandler, ACGame::GameData* gameData);

	private:
		static void ProcessMovements(ACNet::SV_Client& client);

		static void ProcessInteraction(ACNet::SV_NetworkedEntitiesManager* entitiesManager,
			ACNet::SV_MessageHandler* messageHandler,
			ACNet::SV_Client& client,
			GameData* gameData);
		static void ProcessInteraction(ACNet::SV_Client& client,
			ACNet::SV_MessageHandler* messageHandler,
			entt::handle interactionTarget,
		    ACNet::NetworkEntityId networkEntityId,
		    GameData* gameData);
	};
}