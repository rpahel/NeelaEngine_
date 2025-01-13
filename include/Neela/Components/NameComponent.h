#pragma once

#include "Neela/Essentials/Export.h"

#include <string>
#include <nlohmann/json.hpp>

namespace Neela
{
	struct NEELA_ENGINE_API NameComponent
	{
	private:
		std::string m_Name = "";

	public:
		NameComponent() = default;
		NameComponent(const std::string& inName);

		void SetName(const std::string& name);
		const std::string& GetName() const;
	};

	//====================================================================================
	//==== JSON
	//====================================================================================

	inline void to_json(nlohmann::json& j, const NameComponent& nameComponent)
	{
		j["Name"] = nameComponent.GetName();
	}

	inline void from_json(const nlohmann::json& j, NameComponent& nameComponent)
	{
		nameComponent.SetName(j.value("Name", ""));
	}

	inline void to_json(nlohmann::ordered_json& j, const NameComponent& nameComponent)
	{
		j["Name"] = nameComponent.GetName();
	}

	inline void from_json(const nlohmann::ordered_json& j, NameComponent& nameComponent)
	{
		nameComponent.SetName(j.value("Name", ""));
	}
}