#include <catch2/catch_all.hpp>
#include <string>

#include "Common/Inputs/sh_PlayerInputs.h"
#include "Common/Network/sh_NetworkSerializer.h"

TEST_CASE("Serialization")
{
	float original = -163.23f;

	printf("%f\n", original);

	ByteArray data;
	ACNet::NetworkSerializer::Serialize_f(data, original);

    float received = 0.0f;
	std::size_t offset = 0;
	received = ACNet::NetworkSerializer::Deserialize_f(data, offset);

	printf("%f\n", received);

	REQUIRE(original == received);
}

TEST_CASE("Vector Serialization")
{
	std::vector<ACGame::PlayerInputs> sending;
	for (int i = 0; i < 10; ++i)
	{
		auto& inputs = sending.emplace_back();
		inputs.m_Left = true;
		inputs.m_Down = true;
		inputs.m_interactedNetworkEntityId.emplace(50);
	}

	ByteArray byteArray;
	ACNet::NetworkSerializer::Serialize_List(byteArray, sending);

	std::size_t offset = 0;
	std::vector<ACGame::PlayerInputs> received = ACNet::NetworkSerializer::Deserialize_List<ACGame::PlayerInputs>(byteArray, offset);

	REQUIRE(sending.size() == received.size());
	for (int i = 0; i < sending.size(); ++i)
		REQUIRE(sending[i] == received[i]);
}

TEST_CASE("Optional Serialization: empty")
{
	std::optional<ACGame::PlayerInputs> sending;

	ByteArray byteArray;
	ACNet::NetworkSerializer::Serialize_Optional(byteArray, sending);

	std::size_t offset = 0;
	std::optional<ACGame::PlayerInputs> received = ACNet::NetworkSerializer::Deserialize_Optional<ACGame::PlayerInputs>(byteArray, offset);

	REQUIRE(!sending.has_value());
	REQUIRE(sending.has_value() == received.has_value());
}

TEST_CASE("Optional Serialization: not empty")
{
	std::optional<ACGame::PlayerInputs> sending;
	auto& playerInputs = sending.emplace();
	playerInputs.m_interactedNetworkEntityId.emplace(10);
	playerInputs.m_Die = true;
	playerInputs.m_Left = true;

	ByteArray byteArray;
	ACNet::NetworkSerializer::Serialize_Optional(byteArray, sending);

	std::size_t offset = 0;
	std::optional<ACGame::PlayerInputs> received = ACNet::NetworkSerializer::Deserialize_Optional<ACGame::PlayerInputs>(byteArray, offset);

	REQUIRE(sending.has_value() == received.has_value());
	REQUIRE(sending.value() == received.value());
}