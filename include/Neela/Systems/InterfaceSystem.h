#pragma once

#include "Neela/Essentials/Export.h"

#include <entt/entt.hpp>

namespace Neela
{
	class NEELA_ENGINE_API InterfaceSystem
	{
	public:
		static void DrawInterfaces(entt::registry& registry);
	};
}