#pragma once

#include "Common/Character/cl_CharacterCreator.h"

#include <entt/entt.hpp>

namespace ACGame
{
	class MainMenu
	{
	public:
		enum class State
		{
			None = 0,
			Initialisation = 1,
			DefaultScreen = 2,
			CharacterCreator = 3,
			GameStarted = 4,
			Quitted = 5
		};

	private:
		bool m_Host = false;
		std::string m_IpText;
		std::string m_Username = "Connard"; // Toujours une ref aux jeux south park hehe
		MainMenu::State m_CurrentState;
		std::unique_ptr<CharacterCreator> m_CharacterCreator;

		//CharacterAppearance m_characterAppearance;

		entt::handle m_Title;
		entt::handle m_ScreenCharacter;
		entt::handle m_MainMenuMusic;
		entt::handle m_MenuInterface;

	public:
		//==== Constructors

		MainMenu();
		MainMenu(const MainMenu&) = delete;
		MainMenu(MainMenu&&) = default;
		~MainMenu();

		//==== Methods

		void Start();
		void Update();
		void Stop();

		// Loads the most recent character, or default one if none is found.
		void LoadCharacter();

		bool GetShouldHost() const;
		MainMenu::State GetState() const;
		void SetState(MainMenu::State state);
		const std::string& GetUsername() const;
		const std::string& GetIP() const;
		const CharacterAppearance& GetCharacterAppearance() const;

		//==== Operators

		MainMenu& operator=(const MainMenu&) = delete;
		MainMenu& operator=(MainMenu&&) = default;

	private:
		//==== Methods

		void CreateMenuInterface();

		void DrawNameTextBox();

		void DrawHostButton();
		void HostButtonClicked();

		void DrawIpTextBox();
		void DrawConnectButton();
		void ConnectButtonClicked();

		void DrawCreateCharacterButton();
		void CreateCharacterButtonClicked();

		void DrawQuitButton(bool& quit);
		void QuitGame();
	};
}