#include "Neela/Systems/InterfaceSystem.h"
#include "Neela/Components/InterfaceComponent.h"
#include "Neela/Essentials/WindowsManager.h"

#include <imgui.h>

namespace Neela
{
	void Neela::InterfaceSystem::DrawInterfaces(entt::registry& registry)
	{
		auto view = registry.view<InterfaceComponent>();
		for (auto&& [entity, interfaceComp] : view.each())
		{
			if (interfaceComp.m_Instructions && !interfaceComp.m_IsClosed)
				interfaceComp.m_IsClosed = interfaceComp.m_Instructions();
		}

		if(!Neela::WindowsManager::IsMainWindowOpen())
			return;

		ImGui::SetNextFrameWantCaptureMouse(false);
		for (auto&& [entity, interfaceComp] : view.each())
		{
			if (!interfaceComp.m_IsClosed && interfaceComp.m_IsInteractible)
			{
				ImGui::SetNextFrameWantCaptureMouse(true);
				break;
			}
		}
	}
}