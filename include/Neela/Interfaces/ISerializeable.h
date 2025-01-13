#pragma once

#include "Neela/Essentials/Export.h"
#include "Neela/Essentials/Serializer.h"

#include <nlohmann/json.hpp>

namespace Neela
{
	class NEELA_ENGINE_API ISerializeable
	{
	public:
		virtual void Serialize(ByteArray& byteArray) const = 0;
		virtual void Serialize(nlohmann::ordered_json& json) const = 0;

		virtual bool Deserialize(ByteArray& byteArray, std::size_t& offset) = 0;
		virtual bool Deserialize(nlohmann::ordered_json& json) = 0;
	};
}