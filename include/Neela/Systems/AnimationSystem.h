#pragma once

#include "Neela/Essentials/Export.h"

#include <entt/entt.hpp>

namespace Neela
{
	class NEELA_ENGINE_API AnimationSystem
	{
	public:
		static void UpdateAnimations(entt::registry& registry);
	};
}