#pragma once

#include <entt/entt.hpp>

namespace ACNet
{
	class CL_ClientsContainer;
	class CL_MessageHandler;
}

namespace ACGame
{
	struct PlayerInputs;

	class PlayerControllerSystem
	{
	public:
		static void ProcessPlayerInputs(ACNet::CL_ClientsContainer& clientsContainer, ACNet::CL_MessageHandler* msgHandler);
		
		// Usually called by ProcessPlayerInputs.
		// Call it if you want to force movement processing (prediction / reconciliation)
		static void ProcessMovements(entt::handle& handle, const PlayerInputs& inputs, bool affectAnimations = true);
	};
}