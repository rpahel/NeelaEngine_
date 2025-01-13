#pragma once

#include "Neela/Essentials/Export.h"
#include "Neela/Interfaces/IRenderable.h"

#include <memory>

namespace Neela
{
	struct NEELA_ENGINE_API GraphicsComponent
	{
		std::shared_ptr<IRenderable> m_Renderable = nullptr;
		GraphicsComponent(const std::shared_ptr<IRenderable>& renderable);
	};
}