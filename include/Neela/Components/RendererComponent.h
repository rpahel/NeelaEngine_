#pragma once

#include "Neela/Essentials/Export.h"
#include "Neela/Essentials/Renderer.h"

#include <memory>

namespace Neela
{
	struct NEELA_ENGINE_API RendererComponent
	{
		std::shared_ptr<Renderer> m_Renderer = nullptr;
		RendererComponent(const std::shared_ptr<Renderer>& renderer);
	};
}