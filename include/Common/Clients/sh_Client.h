#pragma once 
#include <string>
#include <vector>

#include "Common/Inputs/sh_PlayerInputs.h"
#include "Common/Character/sh_CharacterAppearance.h"

namespace ACNet
{
	struct Client
	{
		std::uint8_t clientId;
		std::string username;

		// Appearance

		ACGame::CharacterAppearance appearance;

		// Inputs

		struct IndexInputsPair
		{
			std::uint32_t index;
			ACGame::PlayerInputs inputs;
		};

		float speed = 100;
		Neela::Vector2f absolutePos;
		IndexInputsPair lastInputs;
		std::uint32_t lastInputIndex = 1;
	};

	template<class T>
	class ClientsContainer
	{
		static_assert(std::is_convertible<T*, Client*>(), "Class must inherit Client");
	protected:
		std::vector<T> m_clients;
	};
}

