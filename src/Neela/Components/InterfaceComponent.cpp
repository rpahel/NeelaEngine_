#include "Neela/Components/InterfaceComponent.h"

namespace Neela
{
	InterfaceComponent::InterfaceComponent(const std::function<bool()>& instructions, bool isInteractible, bool isClosed) :
		m_Instructions(instructions),
		m_IsInteractible(isInteractible),
		m_IsClosed(isClosed)
	{
	}
}