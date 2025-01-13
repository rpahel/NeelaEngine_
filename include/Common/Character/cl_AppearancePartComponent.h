#pragma once

#include "Common/Character/sh_CharacterAppearance.h"

namespace ACGame
{
	struct AppearancePartComponent
	{
		AppearancePart m_AppearancePart;

		AppearancePartComponent() = default;

		AppearancePartComponent(AppearancePart partType) :
			m_AppearancePart(partType)
		{
		}
	};
}