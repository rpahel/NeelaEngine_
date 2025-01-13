#include "Common/Character/sh_CharacterAppearance.h"

namespace ACGame
{
	//====================================================================================
	//==== PUBLIC CONSTRUCTORS
	//====================================================================================

	CharacterAppearance::CharacterAppearance()
	{
		m_Parts =
		{
			{ AppearancePart::Body,		AppearancePartData(1) },
			{ AppearancePart::Hat,		AppearancePartData(0) },
			{ AppearancePart::Hair,		AppearancePartData(0) },
			{ AppearancePart::Glasses,	AppearancePartData(0) },
			{ AppearancePart::Beard,	AppearancePartData(0) },
			{ AppearancePart::Neck,		AppearancePartData(0) },
			{ AppearancePart::Top,		AppearancePartData(0) },
			{ AppearancePart::Bottom,	AppearancePartData(0) },
			{ AppearancePart::Shoes,	AppearancePartData(0) },
		};
	}

	//====================================================================================
	//==== PUBLIC METHODS
	//====================================================================================

	const std::unordered_map<AppearancePart, AppearancePartData>& CharacterAppearance::GetParts() const
	{
		return m_Parts;
	}

	std::uint8_t CharacterAppearance::GetID(AppearancePart part) const
	{
		if (!m_Parts.contains(part))
			return 0;

		return m_Parts.at(part).id;
	}

	void CharacterAppearance::SetID(AppearancePart part, std::uint8_t id)
	{
		if (!m_Parts.contains(part))
		{
			m_Parts.emplace(part, AppearancePartData(id));
		}
		else
		{
			m_Parts.at(part).id = id;
		}
	}

	const Neela::Color& CharacterAppearance::GetColor(AppearancePart part) const
	{
		if (!m_Parts.contains(part))
			return Neela::Color::Clear;

		return m_Parts.at(part).color;
	}

	void CharacterAppearance::SetColor(AppearancePart part, const Neela::Color& color)
	{
		if (!m_Parts.contains(part))
		{
			fmt::print(stderr, fg(fmt::color::red), "CharacterAppearance::SetColor() -> Trying to set the color of a non-existant part.");
			return;
		}
		else
		{
			m_Parts.at(part).color = color;
		}
	}

	std::string CharacterAppearance::AppearancePartToString(AppearancePart part)
	{
		switch (part)
		{
			case ACGame::AppearancePart::None:
				return "None";
			case ACGame::AppearancePart::Body:
				return "Body";
			case ACGame::AppearancePart::Hat:
				return "Hat";
			case ACGame::AppearancePart::Hair:
				return "Hair";
			case ACGame::AppearancePart::Glasses:
				return "Glasses";
			case ACGame::AppearancePart::Beard:
				return "Beard";
			case ACGame::AppearancePart::Neck:
				return "Neck";
			case ACGame::AppearancePart::Top:
				return "Top";
			case ACGame::AppearancePart::Bottom:
				return "Bottom";
			case ACGame::AppearancePart::Shoes:
				return "Shoes";
			default:
				return "None";
		}
	}

	//====================================================================================
	//==== STATIC METHODS
	//====================================================================================

	Neela::SpriteSheet CharacterAppearance::GetDefaultSpriteSheet()
	{
		Neela::SpriteSheet ret;

		// Idle
		Neela::AnimationData iU(Neela::Vector2i(0, 64 * 0), Neela::Vector2i(64), 2, 0.5f);
		Neela::AnimationData iL(Neela::Vector2i(0, 64 * 1), Neela::Vector2i(64), 2, 0.5f);
		Neela::AnimationData iD(Neela::Vector2i(0, 64 * 2), Neela::Vector2i(64), 2, 0.5f);
		Neela::AnimationData iR(Neela::Vector2i(0, 64 * 3), Neela::Vector2i(64), 2, 0.5f);

		// Walk
		Neela::AnimationData wU(Neela::Vector2i(0, 64 * 8), Neela::Vector2i(64), 9, 0.1f);
		Neela::AnimationData wL(Neela::Vector2i(0, 64 * 9), Neela::Vector2i(64), 9, 0.1f);
		Neela::AnimationData wD(Neela::Vector2i(0, 64 * 10), Neela::Vector2i(64), 9, 0.1f);
		Neela::AnimationData wR(Neela::Vector2i(0, 64 * 11), Neela::Vector2i(64), 9, 0.1f);

		// Death
		Neela::AnimationData d1(Neela::Vector2i(0, 64 * 20), Neela::Vector2i(64), 6, 0.25f);
		Neela::AnimationData d2(Neela::Vector2i(64 * 5, 64 * 20), Neela::Vector2i(64), 1, 1);

		ret.SetAnimationsMap(
			{
				{ "IdleUp",		iU },
				{ "IdleLeft",	iL },
				{ "IdleDown",	iD },
				{ "IdleRight",	iR },
				{ "WalkUp",		wU },
				{ "WalkLeft",	wL },
				{ "WalkDown",	wD },
				{ "WalkRight",	wR },
				{ "Dying",		d1 },
				{ "Dead",		d2 }
			}
		);

		ret.PlayAnimation("IdleDown");

		return ret;
	}

	//====================================================================================
	//==== ISERIALIZEABLE
	//====================================================================================

	void CharacterAppearance::Serialize(ByteArray& byteArray) const
	{
		// Nombre de parts
		Neela::Serializer::Serialize<Byte>(byteArray, m_Parts.size());

		for (auto& part : m_Parts)
		{
			Neela::Serializer::Serialize<Byte>(byteArray, static_cast<Byte>(part.first));
			Neela::Serializer::Serialize<Byte>(byteArray, part.second.id);
			part.second.color.Serialize(byteArray);
		}
	}

	void CharacterAppearance::Serialize(nlohmann::ordered_json& json) const
	{
		for (auto& part : m_Parts)
		{
			nlohmann::ordered_json partjson;
			partjson["Type"] = +static_cast<Byte>(part.first);
			partjson["ID"] = +part.second.id;
			part.second.color.Serialize(json["Color"]);
			json.push_back(partjson);
		}
	}

	bool CharacterAppearance::Deserialize(ByteArray& byteArray, std::size_t& offset)
	{
		const std::uint8_t count = Neela::Serializer::Deserialize<std::uint8_t>(byteArray, offset);

		m_Parts.clear();
		m_Parts.reserve(count);

		for (size_t i = 0; i < count; i++)
		{
			const AppearancePart part = static_cast<AppearancePart>(Neela::Serializer::Deserialize<std::uint8_t>(byteArray, offset));

			AppearancePartData data;
			data.id = Neela::Serializer::Deserialize<std::uint8_t>(byteArray, offset);
			data.color.Deserialize(byteArray, offset);

			if (part == AppearancePart::None)
			{
				fmt::print(fg(fmt::color::yellow), "CharacterAppearance::Deserialize() -> \"None\" appearance part detected.");
				continue;
			}

			if (!m_Parts.contains(part))
				m_Parts.emplace(part, data);
			else
				m_Parts.at(part) = data;
		}

		return true;
	}

	bool CharacterAppearance::Deserialize(nlohmann::ordered_json& json)
	{
		m_Parts.clear();
		m_Parts.reserve(json.size());

		for (auto& part : json)
		{
			const AppearancePart a = static_cast<AppearancePart>(part.value("Type", 0));

			if (a == AppearancePart::None)
			{
				fmt::print(fg(fmt::color::yellow), "CharacterAppearance::Deserialize() -> \"None\" appearance part detected.");
				continue;
			}

			AppearancePartData data;
			data.id = part.value("ID", 0);
			data.color.Deserialize(part["Color"]);

			if (!m_Parts.contains(a))
				m_Parts.emplace(a, data);
			else
				m_Parts.at(a) = data;
		}

		return true;
	}

	//====================================================================================
	//==== ISERIALIZEABLE
	//====================================================================================

	void CharacterAppearance::NetSerialize(ByteArray& byteArray) const
	{
		// Nombre de parts
		ACNet::NetworkSerializer::Serialize_u8(byteArray, m_Parts.size());

		for (auto& part : m_Parts)
		{
			ACNet::NetworkSerializer::Serialize_u8(byteArray, static_cast<Byte>(part.first));
			ACNet::NetworkSerializer::Serialize_u8(byteArray, part.second.id);
			ACNet::NetworkSerializer::Serialize_Color(byteArray, part.second.color);
		}
	}

	void CharacterAppearance::NetDeserialize(const ByteArray& byteArray, std::size_t& offset)
	{
		const std::uint8_t count = ACNet::NetworkSerializer::Deserialize_u8(byteArray, offset);

		m_Parts.clear();
		m_Parts.reserve(count);

		for (size_t i = 0; i < count; i++)
		{
			const AppearancePart part = static_cast<AppearancePart>(ACNet::NetworkSerializer::Deserialize_u8(byteArray, offset));

			AppearancePartData data;
			data.id = ACNet::NetworkSerializer::Deserialize_u8(byteArray, offset);
			data.color = ACNet::NetworkSerializer::Deserialize_Color(byteArray, offset);

			if (part == AppearancePart::None)
			{
				fmt::print(fg(fmt::color::yellow), "CharacterAppearance::NetDeserialize() -> \"None\" appearance part detected.");
				continue;
			}

			if (!m_Parts.contains(part))
				m_Parts.emplace(part, data);
			else
				m_Parts.at(part) = data;
		}
	}
}