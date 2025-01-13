#pragma once
#include <entt/entity/handle.hpp>
#include <string>

#include "Common/Agriculture/sh_FarmPlotComponent.h"
#include "Common/Interaction/sh_GenericInteractable.h"
#include "Neela/Graphics/Transform.h"

namespace ACGame
{
	enum class AppearancePart : std::uint8_t;
	struct GridData;

	class EntityCreator
	{
	public:
        static entt::handle CreatePlotFarmEntity(entt::registry& registry, const std::string& name,
                                                 Neela::Vector2f position,
                                                 FarmPlotState state = FarmPlotState::Empty,
                                                 FarmPlantType plantType = FarmPlantType::None,
                                                 float growTimer = 0);

		static entt::handle CreateGenericInteractionCallbackEntity(entt::registry& registry, const std::string& name,
		                                                           Neela::Vector2f position, GenericInteractableEntitiesType type);
	};
}
