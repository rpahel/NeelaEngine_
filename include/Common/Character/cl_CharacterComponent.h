#pragma once

#include "Common/Character/sh_CharacterAppearance.h"
#include "Common/Character/cl_CharacterCreator.h"
#include "Common/Character/cl_AppearancePartComponent.h"
#include "Common/EntityCreators/cl_ClientEntityCreator.h"
#include "Neela/Components/SpriteSheetComponent.h"
#include "Neela/Essentials/WindowsManager.h"
#include "Neela/Essentials/Core.h"
#include "Neela/NeelaConstants.h"

#include <entt/entt.hpp>

namespace ACGame
{
	enum class CharacterOrientation
	{
		None = 0,
		Up = 1,
		Down = 2,
		Left = 3,
		Right = 4
	};

	struct CharacterComponent
	{
		CharacterAppearance m_Appearance;
		CharacterOrientation m_CurrentOrientation = CharacterOrientation::Down;
		std::vector<entt::handle> m_AppearancePartEntities;

		CharacterComponent(const std::string& ownerName, const std::shared_ptr<Neela::Transform>& transform)
		{
			m_AppearancePartEntities.reserve(m_Appearance.GetParts().size());

			for (auto& appearancePart : m_Appearance.GetParts())
			{
				entt::handle part = ACGame::ClientEntityCreator::CreateClientAppearancePartEntity(
					Neela::Core::World(),
					ownerName,
					appearancePart.first,
					transform
				);

				m_AppearancePartEntities.push_back(part);
			}
		}

		~CharacterComponent()
		{
			for (auto& i : m_AppearancePartEntities)
			{
				if(i && i.valid())
					i.destroy();
			}
		}

		void PlayAnimation(const std::string& animationName, bool restartAnimation = false)
		{
			for (auto& i : m_AppearancePartEntities)
			{
				if(!i.valid())
					continue;

				if (Neela::SpriteSheetComponent* ss = i.try_get<Neela::SpriteSheetComponent>())
				{
					if(!ss->m_SpriteSheet)
						continue;

					ss->m_SpriteSheet->PlayAnimation(animationName, restartAnimation);
				}
			}
		}

		entt::handle GetAppearancePart(AppearancePart part)
		{
			for (auto& i : m_AppearancePartEntities)
			{
				if(i.get<AppearancePartComponent>().m_AppearancePart == part)
					return i;
			}

			return entt::handle();
		}

		void UpdateAppearance()
		{
			for (auto& i : m_AppearancePartEntities)
			{
				if(!i || !i.valid())
					continue;

				AppearancePart part = i.get<AppearancePartComponent>().m_AppearancePart;

				std::uint8_t id = m_Appearance.GetID(part);

				Neela::SpriteSheetComponent& sscomp = i.get<Neela::SpriteSheetComponent>();
				std::string spriteNamePath = ACGame::CharacterCreator::GetFileNameForId(part, id);

				sscomp.m_Sprite->SetColor(m_Appearance.GetColor(part));

				if(spriteNamePath.empty())
					sscomp.m_Sprite->SetTexture(nullptr);
				else
					sscomp.m_Sprite->SetTexture(Neela::ResourcesManager::GetTexture(Neela::WindowsManager::GetMainWindow(), spriteNamePath));
			}
		}
	};
}