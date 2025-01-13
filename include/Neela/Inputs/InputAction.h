#pragma once

#include <functional>

namespace Neela
{
	struct InputAction
	{
		std::function<void(bool)> m_CodeToExecute;
		unsigned int m_Count = 0;
	};
}