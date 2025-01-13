#pragma once

#include <cstdint>
#include <string>


namespace ACNet
{
	using NetworkEntityId = std::uint32_t;

	struct NetworkComponent
	{
		NetworkComponent() = default;
		NetworkComponent(NetworkEntityId id);
		NetworkEntityId id;
	};

	inline std::string FormatNetworkedEntityName(std::string name, NetworkEntityId id)
	{
		return name + " (NetId: " + std::to_string(id) + ")";
	}
}
