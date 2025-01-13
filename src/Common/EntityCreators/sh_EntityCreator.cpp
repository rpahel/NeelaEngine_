#include "Common/EntityCreators/sh_EntityCreator.h"
#include "Common/Interaction/sh_InteractableComponent.h"
#include "Common/Interaction/sh_GenericInteractable.h"
#include "Neela/NeelaCore.h"

namespace ACGame
{
	entt::handle EntityCreator::CreatePlotFarmEntity(entt::registry& registry, const std::string& name,
		Neela::Vector2f position, FarmPlotState state, FarmPlantType plantType, float growTimer)
	{
		entt::handle farmPlotEntity = Neela::EntityCreator::CreateEntity(registry, name);
		farmPlotEntity.emplace<Neela::TransformComponent>(std::make_shared<Neela::Transform>(position));
		FarmPlotComponent& farmPlotComponent = farmPlotEntity.emplace<FarmPlotComponent>();
		farmPlotComponent.plantType = plantType;
		farmPlotComponent.state = state;
		farmPlotComponent.growTimer = growTimer;

		return farmPlotEntity;
	}

	entt::handle EntityCreator::CreateGenericInteractionCallbackEntity(entt::registry& registry,
	                                                                   const std::string& name,
	                                                                   Neela::Vector2f position,
	                                                                   GenericInteractableEntitiesType type)
	{
		entt::handle handle = Neela::EntityCreator::CreatePointEntity(registry, name, std::make_shared<Neela::Transform>(position));
		handle.emplace<InteractableComponent>();
		handle.emplace<GenericInteractableComponent>().type = type;
		return handle;
	}
}
