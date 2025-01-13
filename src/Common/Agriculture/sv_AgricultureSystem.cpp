#include "Common/Agriculture/sv_AgricultureSystem.h"

#include "Common/sh_logger.h"
#include "Common/Agriculture/sv_AutomatedComponent.h"
#include "Common/Agriculture/sh_FarmPlantType.h"
#include "Common/Agriculture/sh_FarmPlotComponent.h"
#include "Common/Network/sv_DesyncComponent.h"
#include "Neela/Components/NameComponent.h"

namespace ACGame
{
	void AgricultureSystem::Apply(entt::registry& registry, ACNet::SV_MessageHandler& messageHandler, GameData* gameData, float deltaTime)
	{
		auto classicFarmPlotView = registry.view<FarmPlotComponent, ACNet::NetworkComponent>(entt::exclude<AutomatedComponent>);
		for (auto&& [entity, farmPlotComponent, networkComponent] : classicFarmPlotView.each())
		{
			if(FarmPlantConfigs[farmPlotComponent.plantType].automated) // set farm plot automated if it wasn't
			{
				registry.emplace<AutomatedComponent>(entity);
				continue;
			}
			
			switch (farmPlotComponent.state)
			{
				case FarmPlotState::Empty:
				case FarmPlotState::Grown:
					break;
				case FarmPlotState::Growing:
				{
					assert(farmPlotComponent.plantType != FarmPlantType::None);
					farmPlotComponent.growTimer += deltaTime;
					if (farmPlotComponent.growTimer >= FarmPlantConfigs[farmPlotComponent.plantType].growingDuration)
					{
						farmPlotComponent.state = FarmPlotState::Grown;
						farmPlotComponent.growTimer = 0.0;
						ACLog(ACNet::LogSource::Server, ACNet::LogType::InfoGameState, "Plant '{0}' finished growing a '{1}'\n",
							registry.get<Neela::NameComponent>(entity).GetName(),
							FarmPlantTypeNames[static_cast<std::uint8_t>(farmPlotComponent.plantType)]);
						DesyncComponent::AddComponent(registry, entity);
					}
					break;
				}
				case FarmPlotState::None:
					throw;
				default:;
					throw;
			}
		}

		auto automatedFarmPlotView = registry.view<FarmPlotComponent, AutomatedComponent, ACNet::NetworkComponent>();
		for (auto&& [entity, autoFarmPlot, networkComponent] : automatedFarmPlotView.each())
		{
			switch (autoFarmPlot.state)
			{
			case FarmPlotState::None:
				throw;
			case FarmPlotState::Empty:
				{
					if(autoFarmPlot.plantType == FarmPlantType::None)
						autoFarmPlot.plantType = FarmPlantType::Carrot;
					autoFarmPlot.state = FarmPlotState::Growing;
					autoFarmPlot.growTimer = 0.f;
					DesyncComponent::AddComponent(registry, entity);
					break;
				}
			case FarmPlotState::Growing:
				{
					if(autoFarmPlot.plantType == FarmPlantType::None)
						break;
					autoFarmPlot.growTimer += deltaTime;

					const FarmPlantConfig& farmPlantConfig = FarmPlantConfigs[autoFarmPlot.plantType];
					if(autoFarmPlot.growTimer >= farmPlantConfig.growingDuration)
					{
						autoFarmPlot.growTimer = 0.f;
						autoFarmPlot.state = FarmPlotState::Grown;
						DesyncComponent::AddComponent(registry, entity);
					}
					break;
				}
			case FarmPlotState::Grown:
				{
					autoFarmPlot.growTimer += deltaTime;
					if(autoFarmPlot.growTimer >= 0.3f)
					{
						gameData->AddMoney(FarmPlantConfigs[autoFarmPlot.plantType].harvestGain);
						autoFarmPlot.growTimer = 0.f;
						autoFarmPlot.state = FarmPlotState::Growing;
						DesyncComponent::AddComponent(registry, entity);
					}
					break;
				}
			default: throw;
			}
		}
	}
}
