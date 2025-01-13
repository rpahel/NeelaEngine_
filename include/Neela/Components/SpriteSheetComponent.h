#pragma once

#include "Neela/Essentials/Export.h"
#include "Neela/Essentials/ResourcesManager.h"
#include "Neela/Graphics/Sprite.h"
#include "Neela/Graphics/SpriteSheet.h"

#include <memory>

namespace Neela
{
	struct Rect;

	struct NEELA_ENGINE_API SpriteSheetComponent
	{
		std::shared_ptr<Sprite> m_Sprite = nullptr;
		std::shared_ptr<SpriteSheet> m_SpriteSheet = nullptr;

		SpriteSheetComponent() = default;
		SpriteSheetComponent(std::shared_ptr<Sprite> sprite, std::shared_ptr<SpriteSheet> spriteSheet);
	};

	//====================================================================================
	//==== JSON
	//====================================================================================

	//inline void to_json(nlohmann::json& j, const SpriteSheetComponent& spriteSheetComponent)
	//{
	//	j["Sprite"] = *spriteSheetComponent.m_Sprite;
	//	j["SpriteSheet"] = spriteSheetComponent.m_SpriteSheet ? *spriteSheetComponent.m_SpriteSheet : SpriteSheet();
	//}

	//inline void from_json(const nlohmann::json& j, SpriteSheetComponent& spriteSheetComponent)
	//{
	//	spriteSheetComponent.m_Sprite = std::make_shared<Sprite>(j.value("Sprite", Sprite()));
	//	spriteSheetComponent.m_SpriteSheet = std::make_shared<SpriteSheet>(j.value("SpriteSheet", SpriteSheet()));
	//}

	//inline void to_json(nlohmann::ordered_json& j, const SpriteSheetComponent& spriteSheetComponent)
	//{
	//	j["Sprite"] = *spriteSheetComponent.m_Sprite;
	//	j["SpriteSheet"] = spriteSheetComponent.m_SpriteSheet ? *spriteSheetComponent.m_SpriteSheet : SpriteSheet();
	//}

	//inline void from_json(const nlohmann::ordered_json& j, SpriteSheetComponent& spriteSheetComponent)
	//{
	//	spriteSheetComponent.m_Sprite = std::make_shared<Sprite>(j.value("Sprite", Sprite()));
	//	spriteSheetComponent.m_SpriteSheet = std::make_shared<SpriteSheet>(j.value("SpriteSheet", SpriteSheet()));
	//}
}