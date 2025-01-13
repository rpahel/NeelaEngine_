#include "Neela/Essentials/Game.h"

#include "Common/sh_Constants.h"
#include "Neela/NeelaCore.h"
#include "Common/EntityCreators/sh_EntityCreator.h"

namespace ACGame
{
	void Game::Initialize()
	{
		// Creation de la main window.
		std::weak_ptr<Neela::Window> mainWindow = Neela::WindowsManager::InstantiateWindow("Main", WindowWidth, WindowHeight, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED/*, SDL_WINDOW_BORDERLESS*/);
		mainWindow.lock()->SetDisplayTitle("AC");

		auto mainRendererEntt = Neela::EntityCreator::CreateRendererEntity(
			Neela::Core::World(),
			"MainRenderer",
			mainWindow.lock()->GetRenderer().lock()
		);
		mainRendererEntt.get<Neela::RendererComponent>().m_Renderer->SetBackgroundColor(Neela::Color(0x5c89d1ff));

		// ImGui Current context (a faire dans le .exe. Je l'ai deplace dans le GameClient du coup.)
		// ImGui::SetCurrentContext(mainWindow.lock()->GetImGuiRenderer()->GetContext());

		Neela::Vector2i windowSize = mainWindow.lock()->GetSize();
		// Camera
		auto mainCameraEntt = Neela::EntityCreator::CreateCameraEntity(
			Neela::Core::World(),
			"MainCamera",
			Neela::WindowsManager::GetMainWindow(),
			std::make_shared<Neela::Transform>(windowSize * -0.5f)
		);
	}

	void Game::Shutdown()
	{
		fmt::print("Game::Shutdown().\n");
		Neela::WindowsManager::CloseMainWindow();
	}

	/*void Game::BuildLogosScreen()
	{
		auto creatorLogoEntt = Neela::EntityCreator::CreateSpriteEntity(
			Neela::Core::World(),
			"CreatorLogo",
			Neela::ResourcesManager::GetTexture(Neela::WindowsManager::GetMainWindow(), TEXTURES_ROOT + "Icons/Rpahel.png")
		);
		creatorLogoEntt.get<Neela::SpriteSheetComponent>().m_Sprite->SetColor(Neela::Color::Clear);
		creatorLogoEntt.emplace<Neela::AudioComponent>().AddAudioWave("Effect", Neela::ResourcesManager::GetWav(AUDIO_ROOT + "Voices/MisterRpahel.wav"));

		auto engineLogoEntt = Neela::EntityCreator::CreateSpriteEntity(
			Neela::Core::World(),
			"EngineLogo",
			Neela::ResourcesManager::GetTexture(Neela::WindowsManager::GetMainWindow(), TEXTURES_ROOT + "Icons/Neela.png")
		);
		engineLogoEntt.get<Neela::SpriteSheetComponent>().m_Sprite->SetColor(Neela::Color::Clear);
		engineLogoEntt.emplace<Neela::AudioComponent>().AddAudioWave("Effect", Neela::ResourcesManager::GetWav(AUDIO_ROOT + "Effects/Meow.wav"));

		// Timers des logos
		Neela::TimerSystem::CreateContinuousTimer(
			2,
			[creatorLogoEntt](float dTime, float elapsedTime)
			{
				creatorLogoEntt.get<Neela::SpriteSheetComponent>().m_Sprite->SetColor(
					Neela::Maths::Lerp(
						0xFFFFFF00U,
						0xFFFFFFFFU,
						Neela::Maths::EaseParabolic6(elapsedTime / 2.0)
					));

				if (Neela::Maths::Equals(elapsedTime, 0.10f, 0.01f))
					creatorLogoEntt.get<Neela::AudioComponent>().Play();

				Neela::Transform* transform = creatorLogoEntt.get<Neela::TransformComponent>().m_Transform.get();

				transform->SetRelativeScale(
					Neela::Vector2f(
						Neela::Maths::Lerp(0.25f, 0.75f, Neela::Maths::EaseOutExpo(elapsedTime * 0.5f))
					)
				);

				transform->SetRelativeRotationDeg(Neela::Maths::Lerp(0, 20, Neela::Maths::EaseOutBounce(elapsedTime * 0.5f)));
			},
			[creatorLogoEntt, engineLogoEntt]()
			{
				Neela::TimerSystem::CreateContinuousTimer(
					2,
					[engineLogoEntt](float dTime, float elapsedTime)
					{
						engineLogoEntt.get<Neela::SpriteSheetComponent>().m_Sprite->SetColor(
							Neela::Maths::Lerp(
								0xFFFFFF00U,
								0xFFFFFFFFU,
								Neela::Maths::EaseParabolic6(elapsedTime / 2.0)
							));

						if (Neela::Maths::Equals(elapsedTime, 0.1f, 0.01f))
							engineLogoEntt.get<Neela::AudioComponent>().Play();

						engineLogoEntt.get<Neela::AudioComponent>().SetAudioWaveVolume(
							Neela::Maths::Lerp(0.0f, 0.8f, Neela::Maths::EaseParabolic2(elapsedTime / 2.0f)),
							"Effect"
						);

						engineLogoEntt.get<Neela::TransformComponent>().m_Transform->SetRelativeScale(
							Neela::Vector2f(
								Neela::Maths::Lerp(0.25f, 0.75f, Neela::Maths::EaseOutExpo(elapsedTime * 0.5f))
							)
						);
					},
					[engineLogoEntt]()
					{
						Neela::Core::World().destroy(engineLogoEntt);
						Neela::ResourcesManager::Purge(); // On en aura plus besoin donc on libere la memoire
					},
					1
				);

				Neela::Core::World().destroy(creatorLogoEntt);
			},
			1
		);
	}*/

	/*void Game::BuildTitleScreen(entt::handle mainRendererEntt)
	{
		auto backgroundAudioEntt = Neela::EntityCreator::CreateAudioEntity(Neela::Core::World(), "TitleMusic");
		backgroundAudioEntt.get<Neela::AudioComponent>().AddAudioWave("MainMusic", Neela::ResourcesManager::GetWav(AUDIO_ROOT + "Backgrounds/MainMusic.wav"));

		auto titleEntt = Neela::EntityCreator::CreateSpriteEntity(
			Neela::Core::World(),
			"TitleSprite",
			Neela::ResourcesManager::GetTexture(Neela::WindowsManager::GetMainWindow(), TEXTURES_ROOT + "Icons/Title.png")
		);
		titleEntt.get<Neela::SpriteSheetComponent>().m_Sprite->SetColor(Neela::Color::Clear);
		titleEntt.emplace<Neela::AudioComponent>().AddAudioWave("ThumpEffect", Neela::ResourcesManager::GetWav(AUDIO_ROOT + "Effects/ThumpLowRes.wav"));
		titleEntt.get<Neela::AudioComponent>().AddAudioWave("HeartbeatEffect", Neela::ResourcesManager::GetWav(AUDIO_ROOT + "Effects/Heartbeat.wav"));

		Neela::TimerSystem::CreateContinuousTimer(
			2,
			[mainRendererEntt, backgroundAudioEntt](float dTime, float elapsedTime)
			{
				if (Neela::Maths::Equals(elapsedTime, 1.0f, 0.01f))
				{
					backgroundAudioEntt.get<Neela::AudioComponent>().Play("MainMusic");
					backgroundAudioEntt.get<Neela::AudioComponent>().SetAudioWaveVolume(0.2, "MainMusic");
				}

				float factor = Neela::Maths::Lerp(0.0f, 1.0f, Neela::Maths::EaseOutExpo(elapsedTime / 2.0f));
				mainRendererEntt.get<Neela::RendererComponent>().m_Renderer->SetBackgroundColor(Neela::Color(255 * factor, 255 * factor, 255 * factor));
			},
			[]()
			{
				Neela::WindowsManager::GetMainWindow().lock()->SetDisplayTitle("My Pet Human");
			},
			0
		);

		Neela::TimerSystem::CreateContinuousTimer(
			1,
			[titleEntt](float dTime, float elapsedTime)
			{
				const float factor = Neela::Maths::Lerp(0.0f, 1.0f, Neela::Maths::EaseOutExpo(elapsedTime));
				titleEntt.get<Neela::SpriteSheetComponent>().m_Sprite->SetColor(Neela::Color(255, 255, 255, 255 * factor));

				if (Neela::Maths::Equals(elapsedTime, 0.3f, 0.01f))
					titleEntt.get<Neela::AudioComponent>().Play("ThumpEffect");

				titleEntt.get<Neela::TransformComponent>().m_Transform->SetRelativePosition(
					Neela::Vector2f(
						0,
						Neela::Maths::Lerp(-128.f - 64.f, -10.f, Neela::Maths::EaseOutBounce(elapsedTime))
					)
				);
			},
			[titleEntt]()
			{
				titleEntt.get<Neela::SpriteSheetComponent>().m_Sprite->SetColor(Neela::Color::White);
				titleEntt.get<Neela::TransformComponent>().m_Transform->SetRelativePosition(Neela::Vector2f(0, -10));

				Neela::TimerSystem::CreateContinuousTimer(
					0.25f,
					[titleEntt](float dTime, float elapsedTime)
					{
						if (Neela::Maths::Equals(elapsedTime, 0.0f, 0.02f))
							titleEntt.get<Neela::AudioComponent>().Play("HeartbeatEffect");

						titleEntt.get<Neela::TransformComponent>().m_Transform->SetRelativeScale(
							Neela::Vector2f(Neela::Maths::Lerp(0.9f, 1.0f, Neela::Maths::EaseHeartBeat(elapsedTime * 4)))
						);
					},
					[titleEntt]()
					{
						titleEntt.get<Neela::TransformComponent>().m_Transform->SetRelativeScale(Neela::Vector2f(1));
					},
					0.5f
				);
			},
			1
		);

		Neela::TimerSystem::CreateContinuousTimer(
			2,
			[titleEntt](float dTime, float elapsedTime)
			{
				const float factor = Neela::Maths::Lerp(1.0f, 0.0f, elapsedTime * .5f);
				titleEntt.get<Neela::SpriteSheetComponent>().m_Sprite->SetColor(Neela::Color(255, 255, 255, 255 * factor));
			},
			[titleEntt]()
			{
				Neela::Core::World().destroy(titleEntt);
			},
			5
		);
	}*/
}
