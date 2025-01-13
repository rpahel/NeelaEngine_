#include "Common/Network/sh_NetworkSerializer.h"

#include <enet6/enet.h>
#include <stdexcept>

namespace ACNet
{
	//====================================================================================
	//==== SERIALIZE
	//====================================================================================*

	std::size_t NetworkSerializer::Serialize_u8(ByteArray& byteArray, Byte value)
	{
		std::size_t off = byteArray.size();
		byteArray.resize(off + sizeof(value));
		std::memcpy(&byteArray[off], &value, sizeof(value));
		return off;
	}

	std::size_t NetworkSerializer::Serialize_u8(ByteArray& byteArray, Byte value, std::size_t offset)
	{
		std::memcpy(&byteArray[offset], &value, sizeof(value));
		return offset;
	}

	std::size_t NetworkSerializer::Serialize_u16(ByteArray& byteArray, std::uint16_t value)
	{
		value = htons(value);
		std::size_t off = byteArray.size();
		byteArray.resize(off + sizeof(value));
		std::memcpy(&byteArray[off], &value, sizeof(value));
		return off;
	}

	std::size_t NetworkSerializer::Serialize_u16(ByteArray& byteArray, std::uint16_t value, std::size_t offset)
	{
		value = htons(value);
		std::memcpy(&byteArray[offset], &value, sizeof(value));
		return offset;
	}

	std::size_t NetworkSerializer::Serialize_u32(ByteArray& byteArray, std::uint32_t value)
	{
		value = htonl(value);
		std::size_t off = byteArray.size();
		byteArray.resize(off + sizeof(value));
		std::memcpy(&byteArray[off], &value, sizeof(value));
		return off;
	}

	std::size_t NetworkSerializer::Serialize_u32(ByteArray& byteArray, std::uint32_t value, std::size_t offset)
	{
		value = htonl(value);
		std::memcpy(&byteArray[offset], &value, sizeof(value));
		return offset;
	}

	std::size_t NetworkSerializer::Serialize_s8(ByteArray& byteArray, std::int8_t value)
	{
		std::size_t off = byteArray.size();
		byteArray.resize(off + sizeof(value));
		std::memcpy(&byteArray[off], &value, sizeof(value));
		return off;
	}

	std::size_t NetworkSerializer::Serialize_s8(ByteArray& byteArray, std::int8_t value, std::size_t offset)
	{
		std::memcpy(&byteArray[offset], &value, sizeof(value));
		return offset;
	}

	std::size_t NetworkSerializer::Serialize_s16(ByteArray& byteArray, std::int16_t value)
	{
		value = htons(value);
		std::size_t off = byteArray.size();
		byteArray.resize(off + sizeof(value));
		std::memcpy(&byteArray[off], &value, sizeof(value));
		return off;
	}

	std::size_t NetworkSerializer::Serialize_s16(ByteArray& byteArray, std::int16_t value, std::size_t offset)
	{
		value = htons(value);
		std::memcpy(&byteArray[offset], &value, sizeof(value));
		return offset;
	}

	std::size_t NetworkSerializer::Serialize_s32(ByteArray& byteArray, std::int32_t value)
	{
		value = htonl(value);
		std::size_t off = byteArray.size();
		byteArray.resize(off + sizeof(value));
		std::memcpy(&byteArray[off], &value, sizeof(value));
		return off;
	}

	std::size_t NetworkSerializer::Serialize_s32(ByteArray& byteArray, std::int32_t value, std::size_t offset)
	{
		value = htonl(value);
		std::memcpy(&byteArray[offset], &value, sizeof(value));
		return offset;
	}

	std::size_t NetworkSerializer::Serialize_f(ByteArray& byteArray, float value)
	{
		std::uint32_t htonfreturnsuint32imsostupid = htonf(value);
		std::size_t off = byteArray.size();
		byteArray.resize(off + sizeof(htonfreturnsuint32imsostupid));
		std::memcpy(&byteArray[off], &htonfreturnsuint32imsostupid, sizeof(htonfreturnsuint32imsostupid));
		return off;
	}

	std::size_t NetworkSerializer::Serialize_f(ByteArray& byteArray, float value, std::size_t offset)
	{
		std::uint32_t htonfreturnsuint32imsostupid = htonf(value);
		std::memcpy(&byteArray[offset], &htonfreturnsuint32imsostupid, sizeof(htonfreturnsuint32imsostupid));
		return offset;
	}

	std::size_t NetworkSerializer::Serialize_str(ByteArray& byteArray, const std::string& value)
	{
		std::size_t retOff = Serialize_u16(byteArray, value.size());

		std::size_t off = byteArray.size();
		byteArray.resize(off + value.size());
		std::memcpy(&byteArray[off], &value[0], value.size());
		return retOff;
	}

	// A utiliser prudemment. Deux strings ont rarement exactement le meme nombre de char.
	std::size_t NetworkSerializer::Serialize_str(ByteArray& byteArray, const std::string& value, std::size_t offset)
	{
		std::size_t retOff = Serialize_u16(byteArray, value.size(), offset);
		std::size_t off = retOff + sizeof(std::uint16_t);
		std::memcpy(&byteArray[off], &value[0], value.size());
		return retOff;
	}

	std::size_t NetworkSerializer::Serialize_Vector2f(ByteArray& byteArray, const Neela::Vector2f& value)
	{
		std::size_t result = byteArray.size();
		Serialize_f(byteArray, value.X);
		Serialize_f(byteArray, value.Y);
		return result;
	}

	std::size_t NetworkSerializer::Serialize_Color(ByteArray& byteArray, const Neela::Color& value)
	{
		return Serialize_u32(byteArray, value.ToHex());
	}

	std::size_t NetworkSerializer::Serialize_Color(ByteArray& byteArray, const Neela::Color& value, std::size_t offset)
	{
		return Serialize_u32(byteArray, value.ToHex(), offset);
	}

	std::size_t NetworkSerializer::Serialize_NetworkEntityId(ByteArray& byteArray, NetworkEntityId networkEntityId)
	{
		return Serialize_u32(byteArray, networkEntityId);
	}

	std::size_t NetworkSerializer::Serialize_NetworkEntityId(ByteArray& byteArray, NetworkEntityId networkEntityId,
		std::size_t offset)
	{
		return Serialize_u32(byteArray, networkEntityId, offset);
	}

	//====================================================================================
	//==== DESERIALIZE
	//====================================================================================

	Byte NetworkSerializer::Deserialize_u8(const ByteArray& byteArray, std::size_t& offset)
	{
		if (offset + sizeof(Byte) > byteArray.size())
			throw std::runtime_error("NetworkSerializer::Deserialize_u8() -> Buffer is too small.");

		Byte value = 0;
		std::memcpy(&value, &byteArray[offset], sizeof(Byte));
		offset += sizeof(Byte);

		return value;
	}

	std::uint16_t NetworkSerializer::Deserialize_u16(const ByteArray& byteArray, std::size_t& offset)
	{
		if (offset + sizeof(std::uint16_t) > byteArray.size())
			throw std::runtime_error("NetworkSerializer::Deserialize_u16() -> Buffer is too small.");

		std::uint16_t value = 0;
		std::memcpy(&value, &byteArray[offset], sizeof(std::uint16_t));
		offset += sizeof(std::uint16_t);

		return ntohs(value);
	}

	std::uint32_t NetworkSerializer::Deserialize_u32(const ByteArray& byteArray, std::size_t& offset)
	{
		if (offset + sizeof(std::uint32_t) > byteArray.size())
			throw std::runtime_error("NetworkSerializer::Deserialize_u32() -> Buffer is too small.");

		std::uint32_t value = 0;
		std::memcpy(&value, &byteArray[offset], sizeof(std::uint32_t));
		offset += sizeof(std::uint32_t);

		return ntohl(value);
	}

	std::int8_t NetworkSerializer::Deserialize_s8(const ByteArray& byteArray, std::size_t& offset)
	{
		if (offset + sizeof(std::int8_t) > byteArray.size())
			throw std::runtime_error("NetworkSerializer::Deserialize_s8() -> Buffer is too small.");

		std::int8_t value = 0;
		std::memcpy(&value, &byteArray[offset], sizeof(std::int8_t));
		offset += sizeof(std::int8_t);

		return value;
	}

	std::int16_t NetworkSerializer::Deserialize_s16(const ByteArray& byteArray, std::size_t& offset)
	{
		if (offset + sizeof(std::int16_t) > byteArray.size())
			throw std::runtime_error("NetworkSerializer::Deserialize_s16() -> Buffer is too small.");

		std::int16_t value = 0;
		std::memcpy(&value, &byteArray[offset], sizeof(std::int16_t));
		offset += sizeof(std::int16_t);

		return ntohs(value);
	}

	std::int32_t NetworkSerializer::Deserialize_s32(const ByteArray& byteArray, std::size_t& offset)
	{
		if (offset + sizeof(std::int32_t) > byteArray.size())
			throw std::runtime_error("NetworkSerializer::Deserialize_s32() -> Buffer is too small.");

		std::int32_t value = 0;
		std::memcpy(&value, &byteArray[offset], sizeof(std::int32_t));
		offset += sizeof(std::int32_t);

		return ntohl(value);
	}

	float NetworkSerializer::Deserialize_f(const ByteArray& byteArray, std::size_t& offset)
	{
		if (offset + sizeof(std::uint32_t) > byteArray.size())
			throw std::runtime_error("NetworkSerializer::Deserialize_f() -> Buffer is too small.");

		std::uint32_t value = 0;
		std::memcpy(&value, &byteArray[offset], sizeof(value));
		offset += sizeof(value);

		return ntohf(value);
	}

	std::string NetworkSerializer::Deserialize_str(const ByteArray& byteArray, std::size_t& offset)
	{
		std::uint32_t strSize = Deserialize_u16(byteArray, offset);

		if (offset + strSize > byteArray.size())
			throw std::runtime_error("NetworkSerializer::Deserialize_str() -> Buffer is too small.");

		std::string value(strSize, '\0');
		std::memcpy(&value[0], &byteArray[offset], strSize);
		offset += strSize;

		return value;
	}

	Neela::Vector2f NetworkSerializer::Deserialize_Vector2f(const ByteArray& byteArray, std::size_t& offset)
	{
		Neela::Vector2f result;
		result.X = Deserialize_f(byteArray, offset);
		result.Y = Deserialize_f(byteArray, offset);
		return result;
	}

	Neela::Color NetworkSerializer::Deserialize_Color(const ByteArray& byteArray, std::size_t& offset)
	{
		return Neela::Color::MakeFromHex(Deserialize_u32(byteArray, offset));
	}

	NetworkEntityId NetworkSerializer::Deserialize_NetworkEntityId(const ByteArray& byteArray, std::size_t& offset)
	{
		return Deserialize_u32(byteArray, offset);
	}
}
