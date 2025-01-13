#include <catch2/catch_all.hpp>
#include <string>

#include "Common/Grid/sh_GridData.h"
#include "Common/Network/sh_Protocols.h"

namespace ACNet
{
	TEST_CASE("Default GridData initialization")
	{
		ACGame::GridData gridData;
		gridData.SetSize(10, 10);
		gridData.GenerateRandom();
		REQUIRE(gridData.GetWidth() == 10);
		REQUIRE(gridData.GetHeight() == 10);

		for (int i = 0; i < gridData.GetWidth(); ++i)
		{
			for (int j = 0; j < gridData.GetHeight(); ++j)
			{
				if (i == 0 || i == gridData.GetWidth() - 1
					|| j == 0 || j == gridData.GetHeight() - 1)
					REQUIRE(gridData.GetCellType(i, j) == ACGame::GameBorderCellType);
			}
		}
	}

	TEST_CASE("Sized GridData initialization")
	{
		ACGame::GridData gridData(10, 10);
		REQUIRE(gridData.GetWidth() == 10);
		REQUIRE(gridData.GetHeight() == 10);

		for (int i = 0; i < gridData.GetWidth(); ++i)
		{
			for (int j = 0; j < gridData.GetHeight(); ++j)
			{
				if (i == 0 || i == gridData.GetWidth() - 1
					|| j == 0 || j == gridData.GetHeight() - 1)
					REQUIRE(gridData.GetCellType(i, j) == ACGame::GameBorderCellType);
			}
		}
	}

	TEST_CASE("GridData cell setter")
	{
		ACGame::GridData gridData;
		gridData.SetSize(5, 5);
		gridData.SetCellType(2, 2, ACGame::CellType::Stone);
		REQUIRE(gridData.GetCellType(2, 2) == ACGame::CellType::Stone);
	}

	void TestGridEquality(const ACGame::GridData& a, const ACGame::GridData& b)
	{
		REQUIRE(a.GetWidth() == b.GetWidth());
		REQUIRE(a.GetHeight() == b.GetHeight());
		for (int i = 0; i < a.GetWidth(); ++i)
		{
			for (int j = 0; j < a.GetHeight(); ++j)
			{
				REQUIRE(a.GetCellType(i, j) == b.GetCellType(i, j));
			}
		}
	}

	TEST_CASE("GridData network serialization even")
	{
		ACGame::GridData sent(4, 6);
		sent.GenerateRandom();

		ByteArray byteArray;
		sent.NetSerialize(byteArray);

		ACGame::GridData received;
		std::size_t offset = 0;
		received.NetDeserialize(byteArray, offset);

		TestGridEquality(sent, received);
	}

	TEST_CASE("GridData network serialization odd")
	{
		ACGame::GridData sent(3, 5);
		sent.GenerateRandom();

		ByteArray byteArray;
		sent.NetSerialize(byteArray);

		ACGame::GridData received;
		std::size_t offset = 0;
		received.NetDeserialize(byteArray, offset);

		TestGridEquality(sent, received);
	}

	TEST_CASE("GridData json serialization")
	{
		ACGame::GridData sent(3, 5);
		sent.GenerateRandom();

		nlohmann::ordered_json json;
		sent.Serialize(json);

		ACGame::GridData received;
		received.Deserialize(json);

		TestGridEquality(sent, received);
	}
}
