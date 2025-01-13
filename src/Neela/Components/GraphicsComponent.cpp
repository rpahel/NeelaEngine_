#include "Neela/Components/GraphicsComponent.h"

namespace Neela
{
	GraphicsComponent::GraphicsComponent(const std::shared_ptr<IRenderable>& renderable) : m_Renderable(nullptr)
	{
		m_Renderable = renderable;
	}
}
