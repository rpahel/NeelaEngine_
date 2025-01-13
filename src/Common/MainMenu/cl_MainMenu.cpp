#include "Common/MainMenu/cl_MainMenu.h"

#include "Common/EntityCreators/cl_ClientEntityCreator.h"
#include "Common/Character/cl_CharacterComponent.h"
#include "Common/EntityCreators/sh_EntityCreator.h"
#include "Common/sh_constants.h"

#include "Neela/Components/AudioComponent.h"
#include "Neela/Components/TransformComponent.h"
#include "Neela/Essentials/Core.h"
#include "Neela/Essentials/EntityCreator.h"
#include "Neela/Essentials/WindowsManager.h"
#include "Neela/Essentials/ResourcesManager.h"
#include "Neela/ImGuiRenderer.h"
#include "Neela/Systems/TimerSystem.h"
#include "Neela/NeelaConstants.h"

#define ANIMATIONS_SPEED_FACTOR 0.25f

namespace ACGame
{
	//====================================================================================
	//==== PUBLIC METHODS
	//====================================================================================

	MainMenu::MainMenu()
		: m_CurrentState(State::Initialisation)
	{
	}

	MainMenu::~MainMenu()
	{
	}

	void MainMenu::Start()
	{
		m_CurrentState = State::Initialisation;

		// Character
		LoadCharacter();

		m_ScreenCharacter.get<Neela::TransformComponent>().m_Transform->SetAbsolutePosition(Neela::Vector2f(0, ACGame::WindowHeight * 0.5f + 64));
		Neela::TimerSystem::CreateContinuousTimer(
			3.0f * ANIMATIONS_SPEED_FACTOR,
			[character = &m_ScreenCharacter](float dT, float T)
			{
				Neela::Transform& transform = *character->get<Neela::TransformComponent>().m_Transform;
				transform.SetAbsolutePosition(
					Neela::Maths::Lerp(
						Neela::Vector2f(-64, ACGame::WindowHeight * 0.5f + 64),
						Neela::Vector2f(-64, 0),
						Neela::Maths::EaseOutExpo(T / (3.0f * ANIMATIONS_SPEED_FACTOR))
					)
				);
			},
			2.0f * ANIMATIONS_SPEED_FACTOR,
			[this]()
			{
				m_CurrentState = MainMenu::State::DefaultScreen;
				CreateMenuInterface();
			}
		);

		// Title
		m_Title = Neela::EntityCreator::CreateSpriteEntity(
			Neela::Core::World(),
			"MenuTitle",
			Neela::ResourcesManager::GetTexture(Neela::WindowsManager::GetMainWindow(), TEXTURES_ROOT + "Icons/Title.png")
		);
		m_Title.get<Neela::TransformComponent>().m_Transform->SetAbsolutePosition(Neela::Vector2f(0, -ACGame::WindowHeight * 0.5f - 64));

		Neela::TimerSystem::CreateContinuousTimer(
			3.0f * ANIMATIONS_SPEED_FACTOR,
			[title = &m_Title](float dT, float T)
			{
				Neela::Transform& transform = *title->get<Neela::TransformComponent>().m_Transform;
				transform.SetAbsolutePosition(
					Neela::Maths::Lerp(
						Neela::Vector2f(0, -ACGame::WindowHeight * 0.5f - 64),
						Neela::Vector2f(0, -64),
						Neela::Maths::EaseOutExpo(T / (3.0f * ANIMATIONS_SPEED_FACTOR))
					)
				);
			},
			2.0f * ANIMATIONS_SPEED_FACTOR
		);

		// Main Music
		m_MainMenuMusic = Neela::EntityCreator::CreateAudioEntity(Neela::Core::World(), "MainMenuMusic");
		Neela::AudioComponent& audioComp = m_MainMenuMusic.get<Neela::AudioComponent>();
		audioComp.AddAudioWave("Music", Neela::ResourcesManager::GetWav(AUDIO_ROOT + "Backgrounds/MainMusic.wav"));
		audioComp.Play();
	}

	void MainMenu::Update()
	{
		if (m_CharacterCreator && m_CharacterCreator->GetFinishedEditing())
		{
			m_CharacterCreator.reset();
			CreateMenuInterface();
		}
		else if(m_CharacterCreator && m_CharacterCreator->DirtyCharacter())
		{
			if (m_ScreenCharacter && m_ScreenCharacter.valid())
			{
				m_ScreenCharacter.get<CharacterComponent>().UpdateAppearance();
			}
		}
	}

	void MainMenu::Stop()
	{
		m_CurrentState = State::Initialisation;

		if(m_ScreenCharacter && m_ScreenCharacter.valid())
			m_ScreenCharacter.destroy();

		if (m_MainMenuMusic && m_MainMenuMusic.valid())
			m_MainMenuMusic.destroy();

		if (m_MenuInterface && m_MenuInterface.valid())
			m_MenuInterface.destroy();

		if (m_Title && m_Title.valid())
			m_Title.destroy();
	}

	void MainMenu::LoadCharacter()
	{
		if(m_ScreenCharacter && m_ScreenCharacter.valid())
			m_ScreenCharacter.destroy();

		m_ScreenCharacter = ACGame::ClientEntityCreator::CreateClientCharacterEntity(Neela::Core::World(), "MenuCharacter");
		m_ScreenCharacter.get<Neela::TransformComponent>().m_Transform->SetAbsolutePosition(Neela::Vector2f(-64, 0));

		entt::handle part = m_ScreenCharacter.get<ACGame::CharacterComponent>().GetAppearancePart(ACGame::AppearancePart::Body);
		part.get<Neela::SpriteSheetComponent>().m_Sprite->SetColor(CharacterCreator::DefaultSkinColor);
	}

	bool MainMenu::GetShouldHost() const
	{
		return m_Host;
	}

	MainMenu::State MainMenu::GetState() const
	{
		return m_CurrentState;
	}

	void MainMenu::SetState(MainMenu::State state)
	{
		m_CurrentState = state;
	}

	const std::string& MainMenu::GetUsername() const
	{
		return m_Username;
	}

	const std::string& MainMenu::GetIP() const
	{
		return m_IpText;
	}

	const CharacterAppearance& MainMenu::GetCharacterAppearance() const
	{
		return m_ScreenCharacter.get<CharacterComponent>().m_Appearance;
	}

	//====================================================================================
	//==== PRIVATE METHODS
	//====================================================================================

	void MainMenu::CreateMenuInterface()
	{
		m_MenuInterface = Neela::EntityCreator::CreateInterfaceEntity(
			Neela::Core::World(),
			"MainMenuInterface",
			[this]()
			{
				bool quit = false;

				ImGuiWindowFlags window_flags = 0;
				window_flags |= ImGuiWindowFlags_NoBackground;
				window_flags |= ImGuiWindowFlags_NoDecoration;
				window_flags |= ImGuiWindowFlags_NoMove;
				window_flags |= ImGuiWindowFlags_NoSavedSettings;

				ImVec2 window_pos = ImVec2(ACGame::WindowWidth * 0.5f, ACGame::WindowHeight * 0.5f - 50);
				ImVec2 pivot = ImVec2(0.0f, 0.0f);

				ImGui::SetNextWindowSize(ImVec2(150, 500), ImGuiCond_Always);
				ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, pivot);
				ImGui::Begin("Buttons", NULL, window_flags);
				{
					ImGui::PushStyleColor(21, ImVec4(0.2, 0.2, 0.2, 1));

					DrawNameTextBox();
					DrawCreateCharacterButton();
					DrawHostButton();
					DrawIpTextBox();
					DrawConnectButton();
					DrawQuitButton(quit);

					ImGui::PopStyleColor();
				}
				ImGui::End();

				return quit;
			}
		);
	}

	void MainMenu::DrawNameTextBox()
	{
		ImGui::PushID("NameText");
		ImGui::PushItemWidth(150);

		static char strName[128] = "";
		if (ImGui::InputTextWithHint("", "Your Name", strName, IM_ARRAYSIZE(strName), 32))
			m_Username = std::string(&strName[0], IM_ARRAYSIZE(strName));

		ImGui::PopItemWidth();
		ImGui::PopID();
	}

	void MainMenu::DrawHostButton()
	{
		if (ImGui::Button("Host", ImVec2(150, 30)))
			HostButtonClicked();
	}

	void MainMenu::HostButtonClicked()
	{
		m_Host = true;
		m_CurrentState = State::GameStarted;
	}

	void MainMenu::DrawIpTextBox()
	{
		ImGui::PushID("IpText");
		ImGui::PushItemWidth(150);

		static char strIp[128] = "127.0.0.1";
		if (ImGui::InputTextWithHint("", "IP", strIp, IM_ARRAYSIZE(strIp), 32))
			m_IpText = std::string(&strIp[0], IM_ARRAYSIZE(strIp));

		ImGui::PopItemWidth();
		ImGui::PopID();
	}

	void MainMenu::DrawConnectButton()
	{
		if (ImGui::Button("Connect", ImVec2(150, 30)))
			ConnectButtonClicked();
	}

	void MainMenu::ConnectButtonClicked()
	{
		m_Host = false;
		m_CurrentState = State::GameStarted;
	}

	void MainMenu::DrawCreateCharacterButton()
	{
		if (ImGui::Button("New Character", ImVec2(150, 30)))
			CreateCharacterButtonClicked();
	}

	void MainMenu::CreateCharacterButtonClicked()
	{
		m_CurrentState = State::CharacterCreator;
		m_MenuInterface.destroy();
		m_CharacterCreator = std::make_unique<ACGame::CharacterCreator>();
		m_CharacterCreator->SetCurrentCharacterAppearance(&m_ScreenCharacter.get<CharacterComponent>().m_Appearance);
	}

	void MainMenu::DrawQuitButton(bool& quit)
	{
		if (ImGui::Button("Quit", ImVec2(150, 30)))
		{
			QuitGame();
			quit = true;
		}
	}

	void MainMenu::QuitGame()
	{
		m_CurrentState = State::Quitted;
	}
}