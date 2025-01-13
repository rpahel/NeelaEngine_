#include "Common/Grid/sh_GridData.h"
#include "Common/Network/sh_NetworkSerializer.h"

#include <cassert>

#include "Neela/Maths/Maths.h"

namespace ACGame
{
	//====================================================================================
	//==== CONSTRUCTORS
	//====================================================================================

	GridData::GridData() :
	m_Width(0),
	m_Height(0)
	{
	}

	GridData::GridData(std::uint8_t width, std::uint8_t height)
	{
		SetSize(width, height);
		GenerateRandom();
	}

	//====================================================================================
	//==== METHODS
	//====================================================================================

	void GridData::SetSize(std::uint8_t width, std::uint8_t height)
	{
		m_Width = width;
		m_Height = height;
		m_Cells.resize(m_Width * m_Height);
	}

	void GridData::GenerateRandom()
	{
		for (std::size_t i = 0; i < m_Cells.size(); ++i)
		{
			CellType randomCellType = CellType::None;
			switch (std::abs(Neela::Maths::RandomInt()) % 3)
			{
			case 0:
				randomCellType = CellType::Grass;
				break;
			case 1:
				randomCellType = CellType::Grass2;
				break;
			case 2:
				randomCellType = CellType::Grass3;
				break;
			default: throw;
			}
			m_Cells[i] = randomCellType;
		}

		// Set bottom and top row as border
		for (int i = 0; i < m_Width; ++i)
		{
			m_Cells[i] = GameBorderCellType;
			m_Cells[(m_Height - 1) * m_Width + i] = GameBorderCellType;
		}

		// Set left and right column as border
		for(int i = 0; i < m_Height; ++i)
		{
			m_Cells[i * m_Height] = GameBorderCellType;
			m_Cells[i * m_Height + (m_Width - 1)] = GameBorderCellType;
		}
	}

	CellType GridData::GetCellType(std::uint8_t x, std::uint8_t y) const
	{
		assert(0 <= x && x < m_Width);
		assert(0 <= y && y < m_Height);
		return m_Cells[x + y * m_Width];
	}

	void GridData::SetCellType(std::uint8_t x, std::uint8_t y, CellType cellType)
	{
		assert(0 <= x && x < m_Width);
		assert(0 <= y && y < m_Height);
		m_Cells[x + y * m_Width] = cellType;
	}

	Byte GridData::GetWidth() const
	{
		return m_Width;
	}

	Byte GridData::GetHeight() const
	{
		return m_Height;
	}

	//====================================================================================
	//==== ISERIALIZEABLE
	//====================================================================================

	void GridData::Serialize(ByteArray& byteArray) const
	{
		Neela::Serializer::Serialize<Byte>(byteArray, m_Width);
		Neela::Serializer::Serialize<Byte>(byteArray, m_Height);

		for (CellType cell : m_Cells)
			Neela::Serializer::Serialize<Byte>(byteArray, static_cast<Byte>(cell));
	}

	void GridData::Serialize(nlohmann::ordered_json& json) const
	{
		json["Width"] = m_Width;
		json["Height"] = m_Height;

		nlohmann::ordered_json vector;
		for (CellType cell : m_Cells)
			vector.push_back(+static_cast<Byte>(cell));

		json["Cells"] = vector;
	}

	bool GridData::Deserialize(ByteArray& byteArray, std::size_t& offset)
	{
		m_Width = Neela::Serializer::Deserialize<Byte>(byteArray, offset);
		m_Height = Neela::Serializer::Deserialize<Byte>(byteArray, offset);

		const std::uint16_t a = m_Width; // Peur d'un overflow pendant la multiplication jsp he sayer frer
		const std::uint16_t b = m_Height;

		m_Cells.clear();
		m_Cells.resize(a * b);
		for (size_t i = 0; i < m_Cells.size(); i++)
			m_Cells[i] = static_cast<CellType>(Neela::Serializer::Deserialize<Byte>(byteArray, offset));

		return true;
	}

	bool GridData::Deserialize(nlohmann::ordered_json& json)
	{
		m_Width = json.value("Width", 0);
		m_Height = json.value("Height", 0);
		
		nlohmann::ordered_json vector = json["Cells"];
		m_Cells.clear();
		m_Cells.resize(vector.size());
		for (Byte i = 0; i < m_Cells.size(); ++i)
			m_Cells[i] = static_cast<CellType>(vector[i]);

		return true;
	}

	//====================================================================================
	//==== INETWORKSERIALIZEABLE
	//====================================================================================

	void GridData::NetSerialize(ByteArray& byteArray) const
	{
		ACNet::NetworkSerializer::Serialize_u8(byteArray, m_Width);
		ACNet::NetworkSerializer::Serialize_u8(byteArray, m_Height);

		// On va stocker deux CellType dans un octet.
		// En cas de nombre de CellType impair, on stocke un CellType null pour signifier la fin.
		for (size_t i = 0; i < m_Cells.size(); i += 2) 
		{
			Byte cellTypePair = static_cast<Byte>(m_Cells[i]);
			cellTypePair <<= 4;

			if (i >= m_Cells.size() - 1)
				cellTypePair |= 0;
			else
				cellTypePair |= static_cast<Byte>(m_Cells[i + 1]);

			ACNet::NetworkSerializer::Serialize_u8(byteArray, cellTypePair);
		}
	}

	void GridData::NetDeserialize(const ByteArray& byteArray, std::size_t& offset)
	{
		m_Width = ACNet::NetworkSerializer::Deserialize_u8(byteArray, offset);
		m_Height = ACNet::NetworkSerializer::Deserialize_u8(byteArray, offset);

		const std::uint16_t count = m_Width * m_Height;
		const std::uint16_t size = (count & 1) ? /*impair*/ (count * 0.5f) + 1 : /*pair*/ count * 0.5f;

		m_Cells.clear();
		m_Cells.reserve(count);

		// En cas de nombre de CellType impair, on stocke un CellType null pour signifier la fin.
		for (size_t i = 0; i < size; i++)
		{
			Byte cellTypePair = ACNet::NetworkSerializer::Deserialize_u8(byteArray, offset);

			const CellType a = static_cast<CellType>((cellTypePair >> 4) & 0b1111);
			const CellType b = static_cast<CellType>((cellTypePair >> 0) & 0b1111);

			if(a != CellType::None) // a n'est jamais cense etre None
				m_Cells.push_back(a);

			if (b != CellType::None)
				m_Cells.push_back(b);
		}
	}
}
