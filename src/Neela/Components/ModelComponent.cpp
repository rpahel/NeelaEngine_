#include "Neela/Components/ModelComponent.h"

namespace Neela
{
	ModelComponent::ModelComponent(std::shared_ptr<Model> model) :
		m_Model(std::move(model))
	{
	}
}