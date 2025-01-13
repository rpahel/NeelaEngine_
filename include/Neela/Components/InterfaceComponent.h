#pragma once

#include "Neela/Essentials/Export.h"

#include <functional>

namespace Neela
{
	struct NEELA_ENGINE_API InterfaceComponent
	{
		// Instructions pour dessiner l'interface. Retourne un bool pour savoir elle doit etre fermee
		std::function<bool()> m_Instructions;
		bool m_IsInteractible = true;
		bool m_IsClosed = false;

		InterfaceComponent() = default;
		InterfaceComponent(const std::function<bool()>& instructions, bool isInteractible = true, bool isClosed = false);
	};
}