#pragma once

#include "Neela/Essentials/Export.h"

#include <string>
#include <fmt/core.h>
#include <fmt/color.h>

namespace Neela
{
	class NEELA_ENGINE_API IPrintable
	{
	public:
		virtual const std::string& ToString() const = 0;
	};
}