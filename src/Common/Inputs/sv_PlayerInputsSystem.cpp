#include "Common/Inputs/sv_PlayerInputsSystem.h"
#include "Common/Clients/sv_Client.h"
#include "Common/sh_logger.h"
#include "Common/sh_Constants.h"
#include "Common/Network/sv_NetworkedEntitiesManager.h"
#include "Common/Character/cl_CharacterComponent.h"
#include "Common/Interaction/sh_InteractionCallbackComponent.h"
#include "Common/Network/sv_DesyncComponent.h"
#include "Neela/Components/NameComponent.h"

#include "Neela/Components/TransformComponent.h"

namespace ACGame
{
	void PlayerInputsSystem::ProcessPlayerInputs(ACNet::SV_ClientsContainer* clients,
	                                             ACNet::SV_NetworkedEntitiesManager* entitiesManager,
	                                             ACNet::SV_MessageHandler* messageHandler,
	                                             ACGame::GameData* gameData)
	{
		if(!clients)
			return;

		for (ACNet::SV_Client& client : clients->GetMutableClients())
		{
			if (client.jitterBuffer.empty())
			{
				ACLogWarning(ACNet::LogSource::Server, "Client '{0}' has not inputs!\n", client.clientId);
				continue;
			}

			// Jerome : You stole my code.
			// Me : Yes. :|
			float incr = 1.f;
			const std::size_t jitterBufferSize = client.jitterBuffer.size();
			if (jitterBufferSize < ACNet::SV_TargetJitterBufferSize)
				incr *= 1.f - (ACNet::SV_TargetJitterBufferSize - jitterBufferSize) * 0.05f;
			else if (jitterBufferSize > ACNet::SV_TargetJitterBufferSize)
				incr *= 1.f + (jitterBufferSize - ACNet::SV_TargetJitterBufferSize) * 0.05f;

			client.jitterBufferAdvancement += incr;

			if (client.jitterBufferAdvancement >= 1.f)
			{
				client.lastInputs.inputs.m_Die = false;
				while (client.jitterBufferAdvancement >= 1.f)
				{
					const ACNet::Client::IndexInputsPair& currentInput = client.jitterBuffer.front();
					client.lastInputs.index = currentInput.index;
					client.lastInputs.inputs.m_Die = client.lastInputs.inputs.m_Die || currentInput.inputs.m_Die;
					client.lastInputs.inputs.m_Left = currentInput.inputs.m_Left;
					client.lastInputs.inputs.m_Right = currentInput.inputs.m_Right;
					client.lastInputs.inputs.m_Up = currentInput.inputs.m_Up;
					client.lastInputs.inputs.m_Down = currentInput.inputs.m_Down;

					if(currentInput.inputs.m_interactedNetworkEntityId.has_value())
						client.lastInputs.inputs.m_interactedNetworkEntityId.emplace(currentInput.inputs.m_interactedNetworkEntityId.value());

					client.jitterBuffer.erase(client.jitterBuffer.begin());

					client.jitterBufferAdvancement -= 1.f;
				}
			}

			ProcessMovements(client);
			ProcessInteraction(entitiesManager, messageHandler, client, gameData);
		}
	}

	void PlayerInputsSystem::ProcessMovements(ACNet::SV_Client& client)
	{
		const PlayerInputs& inputs = client.lastInputs.inputs;

		Neela::Vector2f dV;
		dV.X = (int)(inputs.m_Left ^ inputs.m_Right) * ACGame::GameTickDelay * client.speed;
		dV.Y = (int)(inputs.m_Up ^ inputs.m_Down) * ACGame::GameTickDelay * client.speed;

		if (inputs.m_Left)
			dV.X *= -1;

		if (inputs.m_Up)
			dV.Y *= -1;

		if((client.absolutePos.Y <= (TopLeftWindowPosY + ACGame::CellSize) && dV.Y < 0)
			|| (client.absolutePos.Y >= -(TopLeftWindowPosY + ACGame::CellSize) && dV.Y > 0))
			dV.Y = 0;

		if ((client.absolutePos.X <= (TopLeftWindowPosX + ACGame::CellSize) && dV.X < 0)
			|| (client.absolutePos.X >= -(TopLeftWindowPosX + ACGame::CellSize) && dV.X > 0))
			dV.X = 0;

		client.absolutePos += dV;
	}

	void PlayerInputsSystem::ProcessInteraction(ACNet::SV_NetworkedEntitiesManager* entitiesManager,
		ACNet::SV_MessageHandler* messageHandler, ACNet::SV_Client& client, GameData* gameData)
	{
		PlayerInputs& inputs = client.lastInputs.inputs;

		if(!inputs.m_interactedNetworkEntityId.has_value())
			return;

		ACNet::NetworkEntityId networkEntityId = inputs.m_interactedNetworkEntityId.value();
		if(!entitiesManager->HasNetworkedEntity(networkEntityId))
		{
			ACLog(ACNet::LogSource::Server, ACNet::LogType::Warning, "Received client interaction request with an entity which doesn't exist (id: {0})\n", networkEntityId);
			inputs.m_interactedNetworkEntityId.reset();
			return;
		}
		
		entt::handle handle = entitiesManager->GetNetworkedEntity(networkEntityId);
		if(!handle.valid())
		{
			ACLog(ACNet::LogSource::Server, ACNet::LogType::Warning, "Could not retrieve entity with id '{0}'. Cancelled interaction.\n", networkEntityId);
			inputs.m_interactedNetworkEntityId.reset();
			return;
		}

		if(Neela::TransformComponent* targetTransformComponent = handle.try_get<Neela::TransformComponent>())
		{
			float distance = (client.absolutePos - targetTransformComponent->m_Transform->GetAbsolutePosition()).GetMagnitudeSquared();
			if(distance > PlayerInteractionRangeSquared)
			{
				ACLog(ACNet::LogSource::Server, ACNet::LogType::Warning, "Player too far from entity '{0}'. Cancelled interaction.\n", handle.get<Neela::NameComponent>().GetName());
				inputs.m_interactedNetworkEntityId.reset();
				return;
			}
		}
		
		ProcessInteraction(client, messageHandler, handle, networkEntityId, gameData);
		
		inputs.m_interactedNetworkEntityId.reset();
	}

	void PlayerInputsSystem::ProcessInteraction(ACNet::SV_Client& client,
	                                            ACNet::SV_MessageHandler* messageHandler,
	                                            entt::handle interactionTarget,
	                                            ACNet::NetworkEntityId networkEntityId,
	                                            GameData* gameData)
	{
		if (FarmPlotComponent* farmPlot = interactionTarget.try_get<FarmPlotComponent>())
		{
			switch (farmPlot->state)
			{
			case FarmPlotState::Empty:
			{
				FarmPlantType wantedFarmPlot = farmPlot->plantType;
				if(wantedFarmPlot == FarmPlantType::None)
					wantedFarmPlot = FarmPlantType::Carrot;

				std::uint32_t cost = FarmPlantConfigs[wantedFarmPlot].plantationPrice;
				if(!gameData->HasMoney(cost))
					return;
				gameData->RemoveMoney(cost);

				farmPlot->plantType = wantedFarmPlot;
				farmPlot->growTimer = 0.0;
				farmPlot->state = FarmPlotState::Growing;
				ACLog(ACNet::LogSource::Server, ACNet::LogType::InfoGameState, "'{0}' received seeds for '{1}'\n",
					interactionTarget.get<Neela::NameComponent>().GetName(), FarmPlantTypeNames[static_cast<std::uint8_t>(farmPlot->plantType)]);

				DesyncComponent::AddComponent(interactionTarget);
				break;
			}
			case FarmPlotState::Growing:
				break;
			case FarmPlotState::Grown:
			{
				farmPlot->state = FarmPlotState::Empty;
				ACLog(ACNet::LogSource::Server, ACNet::LogType::InfoGameState, "'{0}' got harvested\n", interactionTarget.get<Neela::NameComponent>().GetName());
				gameData->AddMoney(FarmPlantConfigs[farmPlot->plantType].harvestGain);
				DesyncComponent::AddComponent(interactionTarget);
				break;
			}
			default: throw;
			}
			return;
		}

		if(InteractionCallbackComponent* interactionCallbackComponent = interactionTarget.try_get<InteractionCallbackComponent>())
		{
			interactionCallbackComponent->Invoke(interactionTarget, client.clientId);
			return;
		}
	}
}
