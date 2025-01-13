#pragma once

#include "Neela/Essentials/Export.h"
#include "Neela/Interfaces/ISerializeable.h"

#include <vector>
#include <string>
#include <type_traits>
#include <climits>
#include <memory>
#include <nlohmann/json.hpp>
#include <fmt/core.h>
#include <fmt/color.h>

using Byte = std::uint8_t;
using ByteArray = std::vector<Byte>;

namespace Neela
{
	class NEELA_ENGINE_API Serializer
	{
	public :
		//==== Serialize Methods

		template<typename T>
		static std::size_t Serialize(ByteArray& byteArray, const T& value, std::size_t offset);

		template<typename T>
		static std::size_t Serialize(ByteArray& byteArray, T value);
		static std::size_t Serialize(ByteArray& byteArray, const std::string& value);

		//==== Deserialize Methods

		template<typename T>
		static T Deserialize(const ByteArray& byteArray, std::size_t& offset);
	};

	//====================================================================================
	//==== SERIALIZE METHODS
	//====================================================================================

	template<typename T>
	inline std::size_t Serializer::Serialize(ByteArray& byteArray, const T& value, std::size_t offset)
	{
		static_assert(std::is_arithmetic_v<T>, "T must be arithmetic.");

		if (offset == ULLONG_MAX)
		{
			std::size_t off = byteArray.size();
			byteArray.resize(off + sizeof(value));
			std::memcpy(&byteArray[off], &value, sizeof(value));
			return off;
		}
		else
		{
			std::memcpy(&byteArray[offset], &value, sizeof(value));
			return offset;
		}
	}

	template<typename T>
	inline std::size_t Serializer::Serialize(ByteArray& byteArray, T value)
	{
		static_assert(std::is_arithmetic_v<T>, "T must be arithmetic.");

		std::size_t off = byteArray.size();
		byteArray.resize(off + sizeof(value));
		std::memcpy(&byteArray[off], &value, sizeof(value));
		return off;
	}

	inline std::size_t Serializer::Serialize(ByteArray& byteArray, const std::string& value)
	{
		Serialize<std::uint32_t>(byteArray, static_cast<std::uint32_t>(value.size()));

		std::size_t offset = byteArray.size();
		byteArray.resize(offset + value.size());
		std::memcpy(&byteArray[offset], &value[0], value.size());

		return offset;
	}

	template<typename T>
	inline T Serializer::Deserialize(const ByteArray& byteArray, std::size_t& offset)
	{
		static_assert(std::is_arithmetic_v<T>, "T must be arithmetic.");

		if (offset + sizeof(T) > byteArray.size())
			throw std::runtime_error("Serializer::Deserialize() -> Buffer is too small.");

		T value = 0;
		std::memcpy(&value, &byteArray[offset], sizeof(T));
		offset += sizeof(T);

		return value;
	}

	//====================================================================================
	//==== DESERIALIZE METHODS
	//====================================================================================

	template<>
	inline std::string Serializer::Deserialize(const ByteArray& byteArray, std::size_t& offset)
	{
		std::uint32_t strLength = Deserialize<std::uint32_t>(byteArray, offset);

		if (offset + strLength > byteArray.size())
			throw std::runtime_error("Serializer::Deserialize() -> Buffer is too small.");

		std::string value(strLength, '\0');
		std::memcpy(&value[0], &byteArray[offset], strLength);
		offset += strLength;

		return value;
	}
}