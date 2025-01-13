#include <catch2/catch_all.hpp>
#include <string>

#include "Common/Inputs/sh_PlayerInputs.h"

TEST_CASE("Default")
{
	ACGame::PlayerInputs a;
	ACGame::PlayerInputs b;
	REQUIRE(a == b);
	REQUIRE(!a.m_interactedNetworkEntityId.has_value());
}

TEST_CASE("PlayerInputs Serialization test")
{
	ACGame::PlayerInputs sending;
	sending.m_Right = true;
	sending.m_Left = false;
	sending.m_Down = false;
	sending.m_Up = false;
	sending.m_Die = false;

	ByteArray byteArray;
	sending.NetSerialize(byteArray);

	ACGame::PlayerInputs receiving;
	std::size_t offset = 0;
	receiving.NetDeserialize(byteArray, offset);

	REQUIRE(sending == receiving);
}

TEST_CASE("PlayerInputs interacted serialization test")
{
	ACGame::PlayerInputs sending;
	sending.m_Right = true;
	sending.m_Left = true;
	sending.m_Down = false;
	sending.m_Up = false;
	sending.m_Die = false;
	sending.m_interactedNetworkEntityId.emplace(243);

	ByteArray byteArray;
	sending.NetSerialize(byteArray);

	ACGame::PlayerInputs receiving;
	std::size_t offset = 0;
	receiving.NetDeserialize(byteArray, offset);

	REQUIRE(sending == receiving);
}