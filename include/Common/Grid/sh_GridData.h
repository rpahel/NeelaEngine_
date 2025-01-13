#pragma once 
#include "Neela/Interfaces/ISerializeable.h"
#include "Common/Network/sh_INetworkSerializeable.h"

#include <vector>
#include <cstdint>

namespace ACGame
{
	enum class CellType : std::uint8_t
	{
		None = 0,
		Wall = 1,
		InvisibleWall = 2,
		Grass = 3,
		Grass2 = 4,
		Grass3 = 5,
		Dirt = 6,
		Stone = 7,
	};

	constexpr CellType DefaultCellType = CellType::Grass;
	constexpr CellType GameBorderCellType = CellType::InvisibleWall;

	struct GridData : public Neela::ISerializeable, public ACNet::INetworkSerializeable
	{
		//==== Constructors ====

		GridData();
		GridData(std::uint8_t width, std::uint8_t height);

		//==== Methods ====

		void SetSize(std::uint8_t width, std::uint8_t height);
		void GenerateRandom();
		
		CellType GetCellType(std::uint8_t x, std::uint8_t y) const;
		void SetCellType(std::uint8_t x, std::uint8_t y, CellType cellType);

		Byte GetWidth() const;
		Byte GetHeight() const;

		//==== ISerializeable ====

		void Serialize(ByteArray& byteArray) const override;
		void Serialize(nlohmann::ordered_json& json) const override;

		bool Deserialize(ByteArray& byteArray, std::size_t& offset) override;
		bool Deserialize(nlohmann::ordered_json& json) override;

		//==== INetworkSerializeable ====

		void NetSerialize(ByteArray& byteArray) const override;
		void NetDeserialize(const ByteArray& byteArray, std::size_t& offset) override;

	private:
		std::vector<CellType> m_Cells;
		std::uint8_t m_Width;
		std::uint8_t m_Height;
	};
}