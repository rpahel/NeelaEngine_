#include "Neela/Systems/AnimationSystem.h"
#include "Neela/Components/SpriteSheetComponent.h"
#include "Neela/Essentials/Core.h"

namespace Neela
{
	void Neela::AnimationSystem::UpdateAnimations(entt::registry& registry)
	{
		auto view = registry.view<SpriteSheetComponent>();
		for (auto&& [entity, spriteSheetComp] : view.each())
		{
			if (spriteSheetComp.m_SpriteSheet && spriteSheetComp.m_Sprite)
			{
				if(spriteSheetComp.m_SpriteSheet->HasCurrentAnimation())
					spriteSheetComp.m_Sprite->SetSourceRect(spriteSheetComp.m_SpriteSheet->GetSourceRectByTime(Core::GameTime()));
			}
		}
	}
}