#pragma once

#include "Neela/Graphics/Color.h"

#include <vector>
#include <string>

#include "sh_INetworkSerializeable.h"
#include "sh_NetworkComponent.h"
#include "Neela/Maths/Vector.h"

using Byte = std::uint8_t;
using ByteArray = std::vector<Byte>;

namespace ACNet
{
	class NetworkSerializer
	{
	public :
		//==== Serialize

		static std::size_t Serialize_u8(ByteArray& byteArray, Byte value);
		static std::size_t Serialize_u8(ByteArray& byteArray, Byte value, std::size_t offset);
		static std::size_t Serialize_u16(ByteArray& byteArray, std::uint16_t value);
		static std::size_t Serialize_u16(ByteArray& byteArray, std::uint16_t value, std::size_t offset);
		static std::size_t Serialize_u32(ByteArray& byteArray, std::uint32_t value);
		static std::size_t Serialize_u32(ByteArray& byteArray, std::uint32_t value, std::size_t offset);
	
		static std::size_t Serialize_s8(ByteArray& byteArray, std::int8_t value);
		static std::size_t Serialize_s8(ByteArray& byteArray, std::int8_t value, std::size_t offset);
		static std::size_t Serialize_s16(ByteArray& byteArray, std::int16_t value);
		static std::size_t Serialize_s16(ByteArray& byteArray, std::int16_t value, std::size_t offset);
		static std::size_t Serialize_s32(ByteArray& byteArray, std::int32_t value);
		static std::size_t Serialize_s32(ByteArray& byteArray, std::int32_t value, std::size_t offset);
		
		static std::size_t Serialize_f(ByteArray& byteArray, float value);
		static std::size_t Serialize_f(ByteArray& byteArray, float value, std::size_t offset);
		static std::size_t Serialize_str(ByteArray& byteArray, const std::string& value);
		static std::size_t Serialize_str(ByteArray& byteArray, const std::string& value, std::size_t offset);

		static std::size_t Serialize_Vector2f(ByteArray& byteArray, const Neela::Vector2f& value);
		
		static std::size_t Serialize_Color(ByteArray& byteArray, const Neela::Color& value);
		static std::size_t Serialize_Color(ByteArray& byteArray, const Neela::Color& value, std::size_t offset);

		static std::size_t Serialize_NetworkEntityId(ByteArray& byteArray, NetworkEntityId networkEntityId);
		static std::size_t Serialize_NetworkEntityId(ByteArray& byteArray, NetworkEntityId networkEntityId, std::size_t offset);

		template<class T>
		static void Serialize_Optional(ByteArray& byteArray, const std::optional<T>& optional);
		
		template<class T>
		static void Serialize_List(ByteArray& byteArray, const std::vector<T>& list);

		//==== Deserialize

		static Byte				Deserialize_u8(const ByteArray& byteArray, std::size_t& offset);
		static std::uint16_t	Deserialize_u16(const ByteArray& byteArray, std::size_t& offset);
		static std::uint32_t	Deserialize_u32(const ByteArray& byteArray, std::size_t& offset);

		static std::int8_t		Deserialize_s8(const ByteArray& byteArray, std::size_t& offset);
		static std::int16_t		Deserialize_s16(const ByteArray& byteArray, std::size_t& offset);
		static std::int32_t		Deserialize_s32(const ByteArray& byteArray, std::size_t& offset);

		static float			Deserialize_f(const ByteArray& byteArray, std::size_t& offset);
		static std::string		Deserialize_str(const ByteArray& byteArray, std::size_t& offset);

		static Neela::Vector2f	Deserialize_Vector2f(const ByteArray& byteArray, std::size_t& offset);
		
		static Neela::Color		Deserialize_Color(const ByteArray& byteArray, std::size_t& offset);

		static NetworkEntityId	Deserialize_NetworkEntityId(const ByteArray& byteArray, std::size_t& offset);

		template<class T>
		static std::optional<T> Deserialize_Optional(const ByteArray&byteArray, std::size_t& offset);
		
		template<class T>
		static std::vector<T> Deserialize_List(const ByteArray& byteArray, std::size_t& offset);
	};

	template <class T>
	void NetworkSerializer::Serialize_Optional(ByteArray& byteArray, const std::optional<T>& optional)
	{
		bool hasValue = optional.has_value();
		Serialize_u8(byteArray, hasValue);
		if(hasValue)
			optional.value().NetSerialize(byteArray);
	}

	template <class T>
	void NetworkSerializer::Serialize_List(ByteArray& byteArray, const std::vector<T>& list)
	{
		Serialize_u8(byteArray, list.size());
		for (const auto& item : list)
			item.NetSerialize(byteArray);
	}

	template <class T>
	std::optional<T> NetworkSerializer::Deserialize_Optional(const ByteArray& byteArray, std::size_t& offset)
	{
		std::optional<T> result;
		bool hasValue = Deserialize_u8(byteArray, offset);
		if(hasValue)
			result.emplace().NetDeserialize(byteArray, offset);
		else
			result.reset();
		return result;
	}

	template <class T>
	std::vector<T> NetworkSerializer::Deserialize_List(const ByteArray& byteArray, std::size_t& offset)
	{
		std::uint8_t size = Deserialize_u8(byteArray, offset);
		std::vector<T> result(size);
		for (std::uint8_t i = 0; i < size; ++i)
			result[i].NetDeserialize(byteArray, offset);
		return result;
	}
}
