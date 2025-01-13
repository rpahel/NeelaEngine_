#include "Neela/NeelaCore.h"

#include <fmt/core.h>
#include <imgui.h>

namespace Neela
{
	bool InspectorSystem::EntityInspector(entt::handle entity)
	{
		Transform&		transform	= *(entity.get<TransformComponent>().m_Transform);
		const Vector2f	pos			= transform.GetRelativePosition();
		const Vector2f	scale		= transform.GetRelativeScale();
		const float		rot			= transform.GetRelativeRotationDeg();

		std::string windowName = fmt::format("Entity inspector #{}", static_cast<std::uint32_t>(entity.entity()));

		bool shouldClose = false;

		ImGui::Begin(windowName.c_str());
		{
			shouldClose = ImGui::Button("Close");

			transform.PopulateInspector();

			if (SpriteSheetComponent* spritesheet = entity.try_get<SpriteSheetComponent>())
			{
				if(spritesheet->m_Sprite)
					spritesheet->m_Sprite->PopulateInspector();

				if(spritesheet->m_SpriteSheet)
					spritesheet->m_SpriteSheet->PopulateInspector();
			}

			if (ModelComponent* model = entity.try_get<ModelComponent>())
			{
				if (model->m_Model)
					model->m_Model->PopulateInspector();
			}
		}

		ImGui::End();

		return shouldClose;
	}

	entt::handle InspectorSystem::GetEntityAtMousePos(entt::registry& registry, const Transform& camera)
	{
		const Vector2i mousePos = InputsManager::GetMousePosition();
		const Vector2f relativePos(mousePos);

		auto view = registry.view<TransformComponent, GraphicsComponent>();
		for (auto&& [entity, transform, gfx] : view.each())
		{
			const Vector2f worldPos = camera.RelativeToAbsolutePosition(relativePos);
			const Vector2f localPos = transform.m_Transform->AbsoluteToRelativePosition(worldPos);

			const Rect bounds = gfx.m_Renderable->GetBounds();
			if (bounds.Contains(localPos))
			{
				ImGui::SetNextFrameWantCaptureMouse(true);
				return entt::handle(registry, entity);
			}
		}

		return entt::handle{};
	}
}
