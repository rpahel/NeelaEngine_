#include <catch2/catch_all.hpp>
#include <string>

#include "Common/Network/sh_NetworkSerializer.h"
#include "Neela/Maths/Vector.h"

TEST_CASE("Equality")
{
	Neela::Vector2f a(10,10);
	Neela::Vector2f b(10,10);
	REQUIRE(a == b);
}

TEST_CASE("NetSerialization")
{
	Neela::Vector2f sending(15, 8);

	ByteArray byteArray;
	ACNet::NetworkSerializer::Serialize_Vector2f(byteArray, sending);

	std::size_t offset = 0;
	Neela::Vector2f received = ACNet::NetworkSerializer::Deserialize_Vector2f(byteArray, offset);

	REQUIRE(sending == received);
}