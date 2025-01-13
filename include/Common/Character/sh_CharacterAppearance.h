#pragma once

#include "Neela/Graphics/Color.h"
#include "Neela/Interfaces/ISerializeable.h"
#include "Common/Network/sh_INetworkSerializeable.h"
#include "Neela/Graphics/SpriteSheet.h"

#include <unordered_map>
#include <string>

namespace ACGame
{
	enum class AppearancePart : std::uint8_t
	{
		None = 0,
		Body = 1,
		Hat = 2,
		Hair = 3,
		Glasses = 4,
		Beard = 5,
		Neck = 6,
		Top = 7,
		Bottom = 8,
		Shoes = 9,
	};

	struct AppearancePartData
	{
		std::uint8_t id = 0;
		Neela::Color color = Neela::Color::White;

		AppearancePartData() = default;

		AppearancePartData(std::uint8_t inId) :
			id(inId), color(Neela::Color::White)
		{
		}
	};

	class CharacterAppearance : public Neela::ISerializeable, public ACNet::INetworkSerializeable
	{
	private:
		std::unordered_map<AppearancePart, AppearancePartData> m_Parts;

	public:
		//==== Constructors

		CharacterAppearance();

		//==== Methods

		const std::unordered_map<AppearancePart, AppearancePartData>& GetParts() const;

		std::uint8_t GetID(AppearancePart part) const;
		void SetID(AppearancePart part, std::uint8_t id);

		const Neela::Color& GetColor(AppearancePart part) const;
		void SetColor(AppearancePart part, const Neela::Color& color);

		static std::string AppearancePartToString(AppearancePart part);

		//==== Static Methods

		static Neela::SpriteSheet GetDefaultSpriteSheet();

		//==== ISerializeable

		void Serialize(ByteArray& byteArray) const override;
		void Serialize(nlohmann::ordered_json& json) const override;
		bool Deserialize(ByteArray& byteArray, std::size_t& offset) override;
		bool Deserialize(nlohmann::ordered_json& json) override;

		//==== INetworkSerializeable

		void NetSerialize(ByteArray& byteArray) const override;
		void NetDeserialize(const ByteArray& byteArray, std::size_t& offset) override;
	};
}