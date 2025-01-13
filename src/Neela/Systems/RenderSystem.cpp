#include "Neela/Systems/RenderSystem.h"
#include "Neela/Components/TransformComponent.h"
#include "Neela/Components/GraphicsComponent.h"
#include "Neela/Components/RendererComponent.h"
#include "Neela/Components/CameraComponent.h"
#include "Neela/Graphics/Texture.h"
#include "Neela/Essentials/Window.h"
#include "Neela/ImGuiRenderer.h"

#include <algorithm>
#include <tuple>

namespace Neela
{
	void RenderSystem::Draw(entt::registry& registry)
	{
		auto view = registry.view<TransformComponent, GraphicsComponent>();
		const std::size_t size = view.size_hint();
		std::vector<std::tuple<TransformComponent*, GraphicsComponent*>> renderList(size);
		std::size_t i = 0;

		// Bon la c'est clairement pas opti mais j'ai pas le temps de faire autrement.
		// Je pourrais garder un vecteur d'entite ailleurs en memoire de maniere constante que je sort a la creation ou la destruction
		// d'une entite graphique, et au changement d'un renderlayer.
		// Comme ca, j'ai pas besoin de le faire ici a tous les ticks.

		// on mets tout dans la liste
		for (auto&& [entity, transform, graphic] : view.each())
		{
			if(i < size)
				renderList[i++] = std::tuple<TransformComponent*, GraphicsComponent*>(&transform, &graphic);
		}

		// tri des renderlayer
		std::sort(renderList.begin(), renderList.end(),
			[](const std::tuple<TransformComponent*, GraphicsComponent*>& lhs, const std::tuple<TransformComponent*, GraphicsComponent*>& rhs)
			{
				IRenderable* renderableA = get<1>(lhs)->m_Renderable.get();
				IRenderable* renderableB = get<1>(rhs)->m_Renderable.get();
				if (!renderableA || !renderableB)
					return false;
				
				return renderableA->GetRenderLayer() < renderableB->GetRenderLayer();
			}
		);

		// on fait le rendu
		for (auto&& [entity, camtransform, cam] : registry.view<Neela::TransformComponent, Neela::CameraComponent>().each())
		{
			for (auto&& [transformComp, graphicsComp] : renderList)
			{
				if (!graphicsComp->m_Renderable ||
					!graphicsComp->m_Renderable->GetTexture() ||
					!cam.m_Window.lock() ||
					(graphicsComp->m_Renderable->GetTexture()->GetRenderer() != cam.m_Window.lock()->GetRenderer().lock().get()))
					continue;

				Transform transform(
					camtransform.m_Transform->AbsoluteToRelativePosition(transformComp->m_Transform.get()->GetAbsolutePosition()),
					transformComp->m_Transform.get()->GetAbsoluteRotationDeg(),
					transformComp->m_Transform.get()->GetAbsoluteScale()
				);

				graphicsComp->m_Renderable->Draw(transform);
			}
		}
	}

	void RenderSystem::Clear(entt::registry& registry)
	{
		auto view = registry.view<RendererComponent>();
		for (auto&& [entity, rendererComp] : view.each())
		{
			if (!rendererComp.m_Renderer)
				continue;

			rendererComp.m_Renderer->Clear();
		}

		for (auto&& [name, window] : WindowsManager::GetWindows())
			window->GetImGuiRenderer()->NewFrame();
	}

	void RenderSystem::Present(entt::registry& registry)
	{
		for (auto&& [name, window] : WindowsManager::GetWindows())
			window->GetImGuiRenderer()->Render();

		auto view = registry.view<RendererComponent>();
		for (auto&& [entity, rendererComp] : view.each())
		{
			if (!rendererComp.m_Renderer)
				continue;

			rendererComp.m_Renderer->Present();
		}
	}
}