#include "Neela/Essentials/EntityCreator.h"
#include "Neela/Essentials/Components.h"
#include "Neela/Maths/Vector.h"

namespace Neela
{
	entt::handle EntityCreator::CreateEntity(entt::registry& registry, const std::string& name)
	{
		entt::entity entity = registry.create();
		registry.emplace<NameComponent>(entity, name);
		return entt::handle{ registry, entity };
	}

	entt::handle EntityCreator::CreateAudioEntity(entt::registry& registry, const std::string& name)
	{
		entt::handle handle = CreateEntity(registry, name);
		handle.emplace<AudioComponent>();
		return handle;
	}

	entt::handle EntityCreator::CreateInterfaceEntity(entt::registry& registry, const std::string& name, const std::function<bool()>& instructions, bool isInteractible, bool isClosed)
	{
		entt::handle handle = CreateEntity(registry, name);
		handle.emplace<InterfaceComponent>(instructions, isInteractible, isClosed);
		return handle;
	}

	entt::handle EntityCreator::CreateRendererEntity(entt::registry& registry, const std::string& name, std::shared_ptr<Renderer> renderer)
	{
		entt::handle handle = CreateEntity(registry, name);
		handle.emplace<RendererComponent>(renderer);
		return handle;
	}

	entt::handle EntityCreator::CreatePointEntity(entt::registry& registry, const std::string& name, std::shared_ptr<Transform> transform)
	{
		entt::handle handle = CreateEntity(registry, name);
		handle.emplace<TransformComponent>(transform);
		return handle;
	}

	entt::handle EntityCreator::CreateCameraEntity(entt::registry& registry, const std::string& name, const std::weak_ptr<Window>& window, std::shared_ptr<Transform> transform)
	{
		entt::handle handle = CreateEntity(registry, name);
		handle.emplace<TransformComponent>(transform);
		handle.emplace<CameraComponent>(window);
		return handle;
	}

	entt::handle EntityCreator::CreateModelEntity(entt::registry& registry, const std::string& name, std::shared_ptr<Model> model, std::shared_ptr<Transform> transform)
	{
		entt::handle handle = CreateEntity(registry, name);
		handle.emplace<ModelComponent>(model);
		handle.emplace<TransformComponent>(transform);

		// GraphicsComponent prend le shared ptr par copie donc c'est ok ?
		handle.emplace<GraphicsComponent>(handle.get<ModelComponent>().m_Model);

		return handle;
	}

	entt::handle EntityCreator::CreateSpriteEntity(entt::registry& registry, const std::string& name, const std::shared_ptr<Texture>& spriteTexture, std::shared_ptr<SpriteSheet> spriteSheet, std::shared_ptr<Transform> transform)
	{
		entt::handle handle = CreateEntity(registry, name);
		handle.emplace<SpriteSheetComponent>(std::make_shared<Sprite>(spriteTexture), spriteSheet);
		handle.emplace<TransformComponent>(transform);

		// GraphicsComponent prend le shared ptr par copie donc c'est ok ?
		handle.emplace<GraphicsComponent>(handle.get<SpriteSheetComponent>().m_Sprite);

		return handle;
	}
}