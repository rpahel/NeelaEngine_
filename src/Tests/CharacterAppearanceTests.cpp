#include <catch2/catch_all.hpp>
#include <string>

#include "Common/Character/sh_CharacterAppearance.h"

TEST_CASE("Appearance Net Serialization")
{
	ACGame::CharacterAppearance sent;
	sent.SetID(ACGame::AppearancePart::Bottom, 1);
	sent.SetColor(ACGame::AppearancePart::Bottom, Neela::Color::Red);

	ByteArray byteArray;
	sent.NetSerialize(byteArray);

	ACGame::CharacterAppearance received;
	std::size_t offset = 0;
	received.NetDeserialize(byteArray, offset);

	REQUIRE(sent.GetID(ACGame::AppearancePart::Body) == received.GetID(ACGame::AppearancePart::Body));
	REQUIRE(sent.GetID(ACGame::AppearancePart::Bottom) == received.GetID(ACGame::AppearancePart::Bottom));
	REQUIRE(sent.GetColor(ACGame::AppearancePart::Bottom) == received.GetColor(ACGame::AppearancePart::Bottom));
}