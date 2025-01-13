#pragma once

#include "Common/Inputs/sh_PlayerInputs.h"

namespace ACGame
{
	// N'est censé etre présent que sur le character du joueur local.
	struct PlayerControllerComponent
	{
		bool m_IgnoreInputs = false;
		float m_Speed = 100.0f;
		PlayerInputs m_Inputs;
	};
}