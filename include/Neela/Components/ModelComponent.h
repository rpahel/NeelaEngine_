#pragma once

#include "Neela/Essentials/Export.h"
#include "Neela/Graphics/Model.h"

#include <memory>

namespace Neela
{
	struct Rect;

	struct NEELA_ENGINE_API ModelComponent
	{
		std::shared_ptr<Model> m_Model = nullptr;

		ModelComponent() = default;
		ModelComponent(std::shared_ptr<Model> model);
	};

	//====================================================================================
	//==== JSON
	//====================================================================================

	//inline void to_json(nlohmann::json& j, const ModelComponent& modelComponent)
	//{
	//	j["Model"] = *modelComponent.m_Model;
	//}

	//inline void from_json(const nlohmann::json& j, ModelComponent& modelComponent)
	//{
	//	modelComponent.m_Model = std::make_shared<Model>(j.value("Model", Model()));
	//}

	//inline void to_json(nlohmann::ordered_json& j, const ModelComponent& modelComponent)
	//{
	//	j["Model"] = *modelComponent.m_Model;
	//}

	//inline void from_json(const nlohmann::ordered_json& j, ModelComponent& modelComponent)
	//{
	//	modelComponent.m_Model = std::make_shared<Model>(j.value("Model", Model()));
	//}
}