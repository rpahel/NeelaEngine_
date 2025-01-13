#pragma once

#include "Neela/Essentials/Export.h"
#include "Neela/Graphics/Transform.h"
#include "Neela/Graphics/SpriteSheet.h"
#include "Neela/Maths/Vector.h"

#include <entt/entt.hpp>
#include <memory>

namespace Neela
{
	class Renderer;
	class Model;
	class Texture;
	class Window;

	class NEELA_ENGINE_API EntityCreator
	{
	public:
		static entt::handle CreateEntity(entt::registry& registry, const std::string& name);
		static entt::handle CreateAudioEntity(entt::registry& registry, const std::string& name);
		static entt::handle CreateInterfaceEntity(entt::registry& registry, const std::string& name, const std::function<bool()>& instructions, bool isInteractible = true, bool isClosed = false);
		static entt::handle CreateRendererEntity(entt::registry& registry, const std::string& name, std::shared_ptr<Renderer> renderer);
		static entt::handle CreatePointEntity(entt::registry& registry, const std::string& name, std::shared_ptr<Transform> transform = std::make_shared<Transform>());
		static entt::handle CreateCameraEntity(entt::registry& registry, const std::string& name, const std::weak_ptr<Window>& window, std::shared_ptr<Transform> transform = std::make_shared<Transform>());
		static entt::handle CreateModelEntity(entt::registry& registry, const std::string& name, std::shared_ptr<Model> model, std::shared_ptr<Transform> transform = std::make_shared<Transform>());
		static entt::handle CreateSpriteEntity(entt::registry& registry, const std::string& name, const std::shared_ptr<Texture>& spriteTexture, std::shared_ptr<SpriteSheet> spriteSheet = std::make_shared<SpriteSheet>(), std::shared_ptr<Transform> transform = std::make_shared<Transform>());
	};
}