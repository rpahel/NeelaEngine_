#pragma once

#include "Neela/Essentials/Export.h"

#include <entt/entt.hpp>

namespace ACGame
{
	class NEELA_ENGINE_API Game
	{
	private:

	public:
		//==== Constructors ====

		Game() = default;
		Game(const Game&) = delete;
		Game(Game&&) = delete;
		~Game() = default;

		//==== Methods ====

		void Initialize();
		void Shutdown();

		//==== Operators ====

		Game& operator=(const Game&) = delete;
		Game& operator=(Game&&) = delete;

	private:
		//==== Methods ====

		//void BuildLogosScreen();
		//void BuildTitleScreen(entt::handle mainRendererEntt);
		//void BuildPlayerCharacter();
	};
}