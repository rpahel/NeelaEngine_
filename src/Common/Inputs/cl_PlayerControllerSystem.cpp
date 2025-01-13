#include "Common/Character/cl_CharacterComponent.h"
#include "Common/Clients/cl_Client.h"
#include "Common/Inputs/cl_PlayerControllerComponent.h"
#include "Common/Inputs/cl_PlayerControllerSystem.h"
#include "Common/Network/cl_MessageHandler.h"
#include "Common/Network/sh_Protocols.h"
#include "Common/sh_Constants.h"
#include "Common/sh_logger.h"
#include "Neela/Components/TransformComponent.h"
#include "Neela/Essentials/Core.h"

namespace ACGame
{
	void PlayerControllerSystem::ProcessPlayerInputs(ACNet::CL_ClientsContainer& clientsContainer, ACNet::CL_MessageHandler* msgHandler)
	{
		for (auto& client : clientsContainer.GetMutableClients())
		{
			if (!client.handle || !client.handle.valid())
				continue;

			ACGame::PlayerControllerComponent* playerController = client.handle.try_get<ACGame::PlayerControllerComponent>();
			if (!playerController)
				continue;

			if (playerController->m_IgnoreInputs)
				playerController->m_Inputs.Reset();

			// Send inputs trough network
			if (msgHandler)
			{
				ACNet::CL_PlayerInputsPacket packet;
				packet.inputs = playerController->m_Inputs;
				msgHandler->SendMessageToServer(packet, ENET_PACKET_FLAG_RELIABLE);
			}

			ProcessMovements(client.handle, playerController->m_Inputs);

			playerController->m_Inputs.m_interactedNetworkEntityId.reset();
			client.predictedInputs.push_back(
				{
					{ client.lastInputIndex++, playerController->m_Inputs }, // Input
					client.handle.get<Neela::TransformComponent>().m_Transform->GetAbsolutePosition() // Position
				}
			);
		}
	}

	void PlayerControllerSystem::ProcessMovements(entt::handle& handle, const PlayerInputs& inputs, bool affectAnimations)
	{
		Neela::Transform& transform = *handle.get<Neela::TransformComponent>().m_Transform;
		const float speed = handle.get<PlayerControllerComponent>().m_Speed;

		Neela::Vector2f dV;
		dV.X = (int)(inputs.m_Left ^ inputs.m_Right) * ACGame::GameTickDelay * speed;
		dV.Y = (int)(inputs.m_Up ^ inputs.m_Down) * ACGame::GameTickDelay * speed;

		if (inputs.m_Left)
			dV.X *= -1;

		if (inputs.m_Up)
			dV.Y *= -1;

		const Neela::Vector2f currentPos = transform.GetAbsolutePosition();

		if ((currentPos.Y <= (TopLeftWindowPosY + ACGame::CellSize) && dV.Y < 0)
			|| (currentPos.Y >= -(TopLeftWindowPosY + ACGame::CellSize) && dV.Y > 0))
			dV.Y = 0;

		if ((currentPos.X <= (TopLeftWindowPosX + ACGame::CellSize) && dV.X < 0)
			|| (currentPos.X >= -(TopLeftWindowPosX + ACGame::CellSize) && dV.X > 0))
			dV.X = 0;

		transform.AbsoluteTranslate(dV);

		// Animation
		ACGame::CharacterComponent* charaComp = handle.try_get<ACGame::CharacterComponent>();
		if (affectAnimations && charaComp)
		{
			// Les animations de deplacement horizontal ont la priorite sur celles en vertical.
			if (dV.X != 0)
			{
				if (dV.X > 0)
				{
					charaComp->PlayAnimation("WalkRight");
					charaComp->m_CurrentOrientation = CharacterOrientation::Right;
				}
				else
				{
					charaComp->PlayAnimation("WalkLeft");
					charaComp->m_CurrentOrientation = CharacterOrientation::Left;
				}
			}
			else if (dV.Y != 0)
			{
				if (dV.Y < 0)
				{
					charaComp->PlayAnimation("WalkUp");
					charaComp->m_CurrentOrientation = CharacterOrientation::Up;
				}
				else
				{
					charaComp->PlayAnimation("WalkDown");
					charaComp->m_CurrentOrientation = CharacterOrientation::Down;
				}
			}
			else
			{
				switch (charaComp->m_CurrentOrientation)
				{
					case CharacterOrientation::Up:
						charaComp->PlayAnimation("IdleUp");
						break;

					case CharacterOrientation::Down:
						charaComp->PlayAnimation("IdleDown");
						break;

					case CharacterOrientation::Left:
						charaComp->PlayAnimation("IdleLeft");
						break;

					case CharacterOrientation::Right:
						charaComp->PlayAnimation("IdleRight");
						break;

					default:
						break;
				}
			}
		}
	}
}