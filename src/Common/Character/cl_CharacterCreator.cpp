#include "Common/Character/cl_CharacterCreator.h"
#include "Neela/Essentials/EntityCreator.h"
#include "Neela/Essentials/Core.h"
#include "Common/sh_Constants.h"
#include "Common/sh_logger.h"

namespace ACGame
{
	const Neela::Color CharacterCreator::DefaultSkinColor = Neela::Color(0xe8cda2ff);
	std::unordered_map<AppearancePart, std::vector<FileNameID>> CharacterCreator::m_PartToFileName;

	//====================================================================================
	//==== PUBLIC METHODS
	//====================================================================================

	CharacterCreator::CharacterCreator()
	{
		CreateInterface();
		GenerateDefaultFileNames();
	}

	CharacterCreator::~CharacterCreator()
	{
		if (m_CreatorInterface.valid())
			m_CreatorInterface.destroy();
	}

	bool CharacterCreator::GetFinishedEditing() const
	{
		return m_FinishedEditing;
	}

	bool CharacterCreator::DirtyCharacter()
	{
		if (m_DirtyCharacter)
		{
			m_DirtyCharacter = false;
			return true;
		}

		return false;
	}

	void CharacterCreator::SetCurrentCharacterAppearance(CharacterAppearance* charaAppearance)
	{
		m_CurrentCharacterAppearance = charaAppearance;
	}

	void ACGame::CharacterCreator::GenerateDefaultFileNames()
	{
		m_PartToFileName.clear();

		const std::vector<FileNameID> bodyFileNameIDs =
		{
			{ CHARACTER_TEXTURES_ROOT + "Bodies/Body_A.png", 1 }
		};
		m_PartToFileName.emplace(AppearancePart::Body, bodyFileNameIDs);

		const std::vector<FileNameID> hatFileNameIDs =
		{
			{ CHARACTER_TEXTURES_ROOT + "Hats/Hat_A.png", 1 }
		};
		m_PartToFileName.emplace(AppearancePart::Hat, hatFileNameIDs);

		const std::vector<FileNameID> hairFileNameIDs =
		{
			{ CHARACTER_TEXTURES_ROOT + "Hairs/Hair_A.png", 1 }
		};
		m_PartToFileName.emplace(AppearancePart::Hair, hairFileNameIDs);

		const std::vector<FileNameID> glassesFileNameIDs =
		{
			{ CHARACTER_TEXTURES_ROOT + "Glasses/Glasses_A.png", 1 }
		};
		m_PartToFileName.emplace(AppearancePart::Glasses, glassesFileNameIDs);

		const std::vector<FileNameID> beardFileNameIDs =
		{
			{ CHARACTER_TEXTURES_ROOT + "Beards/Beard_A.png", 1 }
		};
		m_PartToFileName.emplace(AppearancePart::Beard, beardFileNameIDs);

		const std::vector<FileNameID> neckFileNameIDs =
		{
			{ CHARACTER_TEXTURES_ROOT + "Necks/Neck_A.png", 1 }
		};
		m_PartToFileName.emplace(AppearancePart::Neck, neckFileNameIDs);

		const std::vector<FileNameID> topFileNameIDs =
		{
			{ CHARACTER_TEXTURES_ROOT + "Tops/Top_A.png", 1 }
		};
		m_PartToFileName.emplace(AppearancePart::Top, topFileNameIDs);

		const std::vector<FileNameID> bottomFileNameIDs =
		{
			{ CHARACTER_TEXTURES_ROOT + "Bottoms/Bottom_A.png", 1 }
		};
		m_PartToFileName.emplace(AppearancePart::Bottom, bottomFileNameIDs);

		const std::vector<FileNameID> shoesFileNameIDs =
		{
			{ CHARACTER_TEXTURES_ROOT + "Shoes/Shoes_A.png", 1 }
		};
		m_PartToFileName.emplace(AppearancePart::Shoes, shoesFileNameIDs);
	}

	std::string CharacterCreator::GetFileNameForId(AppearancePart part, std::uint8_t id)
	{
		if (m_PartToFileName.empty())
			GenerateDefaultFileNames();

		const std::vector<FileNameID>& vector = m_PartToFileName.at(part);

		id = id % (vector.size() + 1);

		if (id == 0 && part == AppearancePart::Body)
			id = 1;

		if (id == 0)
			return "";

		for (auto& i : vector)
		{
			if (i.id == id)
				return i.fileName;
		}

		return "";
	}

	//====================================================================================
	//==== PRIVATE METHODS
	//====================================================================================

	void CharacterCreator::NextButtonClicked(AppearancePart part)
	{
		if (!m_CurrentCharacterAppearance)
		{
			printf("CharacterCreator::NextButtonClicked() -> m_CurrentCharacterAppearance is null.\n");
			return;
		}

		m_CurrentCharacterAppearance->SetID(part, m_CurrentCharacterAppearance->GetID(part) + 1);
		m_DirtyCharacter = true;
	}

	void CharacterCreator::ConfirmButtonClicked()
	{
		m_FinishedEditing = true;

		// TODO : Sauvegarder le personnage dans un fichier avec son nom et son apparance
	}

	void CharacterCreator::CreateInterface()
	{
		m_CreatorInterface = Neela::EntityCreator::CreateInterfaceEntity(
			Neela::Core::World(),
			"MainMenuInterface",
			[this]()
			{
				bool quit = false;

				if (!m_CurrentCharacterAppearance)
					return true;

				ImGuiWindowFlags window_flags = 0;
				window_flags |= ImGuiWindowFlags_NoBackground;
				window_flags |= ImGuiWindowFlags_NoDecoration;
				window_flags |= ImGuiWindowFlags_NoMove;
				window_flags |= ImGuiWindowFlags_NoSavedSettings;

				ImVec2 window_pos = ImVec2(ACGame::WindowWidth * 0.5f, ACGame::WindowHeight * 0.5f - 50);
				ImVec2 pivot = ImVec2(0.0f, 0.0f);

				ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, pivot);
				ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_Always);
				ImGui::Begin("Buttons", NULL, window_flags);
				{
					ImGui::PushStyleColor(21, ImVec4(0.2, 0.2, 0.2, 1));

					ImGui::SameLine(40);
					ImGui::Text("Red");
					ImGui::SameLine(40 + 100);
					ImGui::Text("Green");
					ImGui::SameLine(40 + 200);
					ImGui::Text("Blue");

					// je suis debile

					for (size_t i = 1; i < 10; i++)
					{
						std::string name = CharacterAppearance::AppearancePartToString((AppearancePart)i);
						if (ImGui::ArrowButton(name.c_str(), ImGuiDir_Right))
							NextButtonClicked((AppearancePart)i);

						float r = (float)m_CurrentCharacterAppearance->GetColor((AppearancePart)i).R / 255.f;
						float g = (float)m_CurrentCharacterAppearance->GetColor((AppearancePart)i).G / 255.f;
						float b = (float)m_CurrentCharacterAppearance->GetColor((AppearancePart)i).B / 255.f;

						float rgb[3] = {r, g, b};

						ImGui::SameLine();
						ImGui::PushID(name.c_str());
						ImGui::SetNextItemWidth(300);
						if (ImGui::ColorEdit3("", &rgb[0]))
						{
							m_CurrentCharacterAppearance->SetColor((AppearancePart)i, Neela::Color(rgb[0] * 255, rgb[1] * 255, rgb[2] * 255));
							m_DirtyCharacter = true;
						}
						ImGui::PopItemWidth();
						ImGui::PopID();
						ImGui::SameLine();

						ImGui::Text(name.c_str());
					}

					if (ImGui::Button("Confirm", ImVec2(150, 30)))
					{
						ConfirmButtonClicked();
						quit = true;
					}

					ImGui::PopStyleColor();
				}
				ImGui::End();

				return quit;
			}
		);
	}
}