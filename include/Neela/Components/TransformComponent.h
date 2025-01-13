#pragma once

#include "Neela/Essentials/Export.h"
#include "Neela/Graphics/Transform.h"

#include <memory>

namespace Neela
{
	struct NEELA_ENGINE_API TransformComponent
	{
		std::shared_ptr<Transform> m_Transform = nullptr;

		TransformComponent() = default;
		TransformComponent(std::shared_ptr<Transform> transform);
	};

	//====================================================================================
	//==== JSON
	//====================================================================================

	//inline void to_json(nlohmann::json& j, const TransformComponent& transformComponent)
	//{
	//	j["Transform"] = *transformComponent.m_Transform;
	//}

	//inline void from_json(const nlohmann::json& j, TransformComponent& transformComponent)
	//{
	//	transformComponent.m_Transform = std::make_shared<Transform>(j["Transform"].template get<Transform>());
	//}

	//inline void to_json(nlohmann::ordered_json& j, const TransformComponent& transformComponent)
	//{
	//	j["Transform"] = *transformComponent.m_Transform;
	//}

	//inline void from_json(const nlohmann::ordered_json& j, TransformComponent& transformComponent)
	//{
	//	transformComponent.m_Transform = std::make_shared<Transform>(j["Transform"].template get<Transform>());
	//}
}