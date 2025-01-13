#include "Neela/Components/TransformComponent.h"

namespace Neela
{
	TransformComponent::TransformComponent(std::shared_ptr<Transform> transform) : m_Transform(std::move(transform)) {}
}