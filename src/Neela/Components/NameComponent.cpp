#include "Neela/Components/NameComponent.h"
#include "Neela/Essentials/Core.h"

#include "entt/entt.hpp"
#include "fmt/core.h"
#include "fmt/color.h"

namespace Neela
{
	NameComponent::NameComponent(const std::string& inName)
	{
		SetName(inName);
	}

	void NameComponent::SetName(const std::string& name)
	{
		for (auto&& [entity, nameComp] : Core::World().view<NameComponent>().each())
		{
			if (name == nameComp.m_Name)
			{
				fmt::print(fg(fmt::color::red), "NameComponent::SetName() -> A Component of name \"{0}\" already exists.\n", name);
				return;
			}
		}

		m_Name = name;
	}

	const std::string& NameComponent::GetName() const
	{
		return m_Name;
	}
}