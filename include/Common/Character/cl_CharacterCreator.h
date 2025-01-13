#pragma once

#include "Neela/Graphics/Color.h"
#include "Common/Character/sh_CharacterAppearance.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <entt/entt.hpp>

namespace ACGame
{
	struct FileNameID
	{
		std::string fileName;
		std::uint8_t id;
	};

	class CharacterCreator
	{
	public:
		// Beige un peu terne.
		static const Neela::Color DefaultSkinColor;

	private:
		static std::unordered_map<AppearancePart, std::vector<FileNameID>> m_PartToFileName;

		CharacterAppearance* m_CurrentCharacterAppearance;
		bool m_FinishedEditing = false;
		bool m_DirtyCharacter = false;
		std::string m_Username = "Connard"; // Toujours une ref aux jeux south park hehe

		entt::handle m_CreatorInterface;

	public:
		//==== Constructors

		CharacterCreator();
		CharacterCreator(const CharacterCreator&) = delete;
		CharacterCreator(CharacterCreator&&) = default;
		~CharacterCreator();

		//==== Methods

		bool GetFinishedEditing() const;
		bool DirtyCharacter();
		void SetCurrentCharacterAppearance(CharacterAppearance* charaAppearance);

		static std::string GetFileNameForId(AppearancePart part, std::uint8_t id);

		//==== Operators

		CharacterCreator& operator=(const CharacterCreator&) = delete;
		CharacterCreator& operator=(CharacterCreator&&) = default;

	private:
		//==== Methods

		void NextButtonClicked(AppearancePart part);
		void ConfirmButtonClicked();
		void CreateInterface();

		static void GenerateDefaultFileNames();
	};
}