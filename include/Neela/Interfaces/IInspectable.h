#pragma once

#include "Neela/Essentials/Export.h"

#include <imgui.h>

namespace Neela
{
	class NEELA_ENGINE_API IInspectable
	{
	public:
		virtual void PopulateInspector() = 0;
	};
}