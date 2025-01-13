#pragma once

#include "Neela/Essentials/Export.h"

#include <entt/entt.hpp>

namespace Neela
{
	class Transform;

	class NEELA_ENGINE_API InspectorSystem
	{
	public:
		static bool EntityInspector(entt::handle entity);
		static entt::handle GetEntityAtMousePos(entt::registry& registry, const Transform& camera);
	};
}