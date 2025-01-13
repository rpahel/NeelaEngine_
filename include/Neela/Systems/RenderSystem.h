#pragma once

#include "Neela/Essentials/Export.h"
#include "Neela/Interfaces/IRenderable.h"

#include <entt/entt.hpp>

namespace Neela
{
	class NEELA_ENGINE_API RenderSystem
	{
	public:
		static void Draw(entt::registry& registry);
		static void Clear(entt::registry& registry);
		static void Present(entt::registry& registry);
	};
}