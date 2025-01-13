#pragma once

#include "Neela/Essentials/Export.h"
#include "Neela/Essentials/WindowsManager.h"
#include "Neela/Essentials/Window.h"

#include <memory>
#include <nlohmann/json.hpp>

namespace Neela
{
	struct NEELA_ENGINE_API CameraComponent
	{
		std::weak_ptr<Window> m_Window;

		explicit CameraComponent(const std::weak_ptr<Window>& window = std::weak_ptr<Window>()) : m_Window(window) {}
	};

	//====================================================================================
	//==== JSON
	//====================================================================================

	inline void to_json(nlohmann::json& j, const CameraComponent& cameraComponent)
	{
		j["Window"] = cameraComponent.m_Window.lock() ? cameraComponent.m_Window.lock()->GetTitle() : "";
	}

	inline void from_json(const nlohmann::json& j, CameraComponent& cameraComponent)
	{
		std::string title = j.value("Window", "");
		assert(title != "");
		cameraComponent.m_Window = WindowsManager::GetWindowByTitle(title);
	}

	inline void to_json(nlohmann::ordered_json& j, const CameraComponent& cameraComponent)
	{
		j["Window"] = cameraComponent.m_Window.lock() ? cameraComponent.m_Window.lock()->GetTitle() : "";
	}

	inline void from_json(const nlohmann::ordered_json& j, CameraComponent& cameraComponent)
	{
		std::string title = j.value("Window", "");
		assert(title != "");
		cameraComponent.m_Window = WindowsManager::GetWindowByTitle(title);
	}
}