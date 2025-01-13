#include "Common/EntityCreators/cl_ClientEntityCreator.h"

#include "Common/Character/cl_CharacterComponent.h"
#include "Common/Grid/cl_RenderableGrid.h"
#include "Common/Inputs/cl_PlayerControllerComponent.h"
#include "Neela/NeelaConstants.h"
#include "Neela/Components/GraphicsComponent.h"
#include "Neela/Components/TransformComponent.h"
#include "Neela/Essentials/EntityCreator.h"
#include "Neela/Essentials/WindowsManager.h"

namespace ACGame
{
	entt::handle ClientEntityCreator::CreateClientCharacterEntity(entt::registry& registry, const std::string& name,
		std::shared_ptr<Neela::Transform> transform)
	{
		auto entity = Neela::EntityCreator::CreatePointEntity(registry, name + "_Root", transform);
		entity.emplace<CharacterComponent>(name, transform);
		return entity;
	}

	entt::handle ClientEntityCreator::CreateClientAppearancePartEntity(entt::registry& registry,
		const std::string& name, AppearancePart partType,
		std::shared_ptr<Neela::Transform> transform)
	{
		std::string path = "INVALID_TEXTURE";
		std::string partName = "NONE";
		int zIndex = 0;

		switch (partType)
		{
			case ACGame::AppearancePart::None:
				fmt::print(stderr, fg(fmt::color::red),
					"EntityCreator::CreateAppearancePart() -> AppearancePart of type None detected.\n");
				break;

			case ACGame::AppearancePart::Body:
				path = "Bodies/Body_A.png";
				partName = "Body";
				zIndex = 100;
				break;

			case ACGame::AppearancePart::Hat:
				path = "";
				partName = "Hat";
				zIndex = 108;
				break;

			case ACGame::AppearancePart::Hair:
				path = "";
				partName = "Hair";
				zIndex = 107;
				break;

			case ACGame::AppearancePart::Glasses:
				path = "";
				partName = "Glasses";
				zIndex = 106;
				break;

			case ACGame::AppearancePart::Beard:
				path = "";
				partName = "Beard";
				zIndex = 105;
				break;

			case ACGame::AppearancePart::Neck:
				path = "";
				partName = "Neck";
				zIndex = 104;
				break;

			case ACGame::AppearancePart::Top:
				path = "";
				partName = "Top";
				zIndex = 102;
				break;

			case ACGame::AppearancePart::Bottom:
				path = "";
				partName = "Bottom";
				zIndex = 101;
				break;

			case ACGame::AppearancePart::Shoes:
				path = "";
				partName = "Shoes";
				zIndex = 103;
				break;

			default:
				fmt::print(stderr, fg(fmt::color::red),
					"EntityCreator::CreateAppearancePart() -> AppearancePart of unexpected type detected.\n");
				break;
		}

		std::shared_ptr<Neela::Texture> defaultTexture =
			path.empty() ?
			nullptr
			: Neela::ResourcesManager::GetTexture(Neela::WindowsManager::GetMainWindow(), CHARACTER_TEXTURES_ROOT + path);

		auto entity = Neela::EntityCreator::CreateSpriteEntity(
			registry,
			name + "_" + partName,
			defaultTexture,
			std::make_shared<Neela::SpriteSheet>(std::move(CharacterAppearance::GetDefaultSpriteSheet())),
			std::make_shared<Neela::Transform>(transform.get())
		);

		entity.get<Neela::SpriteSheetComponent>().m_Sprite->SetRenderLayer(zIndex);
		entity.get<Neela::SpriteSheetComponent>().m_Sprite->SetSize(128, 128);

		entity.emplace<ACGame::AppearancePartComponent>(partType);

		return entity;
	}

	entt::handle ClientEntityCreator::CreateClientPlayerCharacterEntity(entt::registry& registry,
		const std::string& name,
		std::shared_ptr<Neela::Transform> transform)
	{
		auto entity = CreateClientCharacterEntity(registry, name, transform);
		entity.emplace<ACGame::PlayerControllerComponent>();
		return entity;
	}

	entt::handle ClientEntityCreator::CreateClientGridEntity(entt::registry& registry, const std::string& name,
		Neela::Vector2f position, const ACGame::GridData* gridData)
	{
		assert(gridData);

		entt::handle entity = Neela::EntityCreator::CreateEntity(registry, name);
		entity.emplace<Neela::TransformComponent>(std::make_shared<Neela::Transform>(position));
		auto renderableGrid = std::make_shared<ACGame::RenderableGrid>(gridData);
		renderableGrid->SetRenderLayer(1);
		entity.emplace<Neela::GraphicsComponent>(renderableGrid);

		return entity;
	}
}
