#pragma once

#include "Neela/Components/SpriteSheetComponent.h"

namespace Neela
{
	SpriteSheetComponent::SpriteSheetComponent(std::shared_ptr<Sprite> sprite, std::shared_ptr<SpriteSheet> spriteSheet) :
		m_Sprite(std::move(sprite)), m_SpriteSheet(std::move(spriteSheet))
	{
	}
}
