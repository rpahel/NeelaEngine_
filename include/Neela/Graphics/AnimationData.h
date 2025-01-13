#pragma once

#include "Neela/Essentials/Export.h"
#include "Neela/Maths/Vector.h"
#include "Neela/Maths/Rect.h"
#include "Neela/Interfaces/IPrintable.h"
#include "Neela/Interfaces/IInspectable.h"
#include "Neela/Interfaces/ISerializeable.h"

#define ANIMATIONDATA_SERIALIZE_VERSION 1

namespace Neela
{
	struct NEELA_ENGINE_API AnimationData : IPrintable, IInspectable, ISerializeable
	{
		Vector2i	m_PositionInTexture;
		Vector2i	m_FrameSize;
		float		m_FrameDuration;
		uint8_t		m_FramesCount;
		uint8_t		m_CurrentFrameIndex;

		//==== Constructors =====

		AnimationData() : AnimationData(Vector2i(0), Vector2i(0), 0) {}

		AnimationData(const Vector2i& textureSize, std::uint8_t framesCount, float frameDuration = 0.25f) :
			m_PositionInTexture(Vector2i()), m_FramesCount(framesCount), m_FrameDuration(frameDuration), m_CurrentFrameIndex(0), m_FrameSize(textureSize / Vector2i(framesCount, 1))
		{
		}

		AnimationData(Vector2i positionInTexture, Vector2i frameSize, std::uint8_t framesCount, float frameDuration = 0.25f) :
			m_PositionInTexture(positionInTexture), m_FrameSize(frameSize), m_FrameDuration(frameDuration), m_FramesCount(framesCount), m_CurrentFrameIndex(0)
		{
		}

		//==== Methods ====

		Rect GetCurrentFrameRect() const
		{
			return Rect(
				m_FrameSize.X,
				m_FrameSize.Y,
				m_PositionInTexture.X + m_CurrentFrameIndex * m_FrameSize.X,
				m_PositionInTexture.Y
			);
		}

		//==== IInspectable ====

		void PopulateInspector() override
		{
			if (!ImGui::TreeNode("Animation data"))
				return;

			ImGui::Text("Current frame index : %u", m_CurrentFrameIndex);

			int posArray[2] = { m_PositionInTexture.X, m_PositionInTexture.Y };
			if (ImGui::DragInt2("Texture Coords", posArray))
				m_PositionInTexture = Vector2i(posArray[0], posArray[1]);

			int sizeArray[2] = { m_FrameSize.X, m_FrameSize.Y };
			if (ImGui::DragInt2("Frame Size", sizeArray))
				m_FrameSize = Vector2i(sizeArray[0], sizeArray[1]);

			float duration = m_FrameDuration;
			if (ImGui::DragFloat("Frame Duration", &duration, 0.01f, 0, 10))
				m_FrameDuration = duration;

			int framesCount = m_FramesCount;
			if (ImGui::InputInt("Frames Count", &framesCount))
				m_FramesCount = framesCount;

			ImGui::TreePop();
		}

		//==== ISerializeable ====

		void Serialize(std::vector<uint8_t>& byteArray) const override
		{
			Serializer::Serialize<uint8_t>(byteArray, ANIMATIONDATA_SERIALIZE_VERSION);
			m_PositionInTexture.Serialize(byteArray);
			m_FrameSize.Serialize(byteArray);
			Serializer::Serialize<uint8_t>(byteArray, m_FramesCount);
			Serializer::Serialize<float>(byteArray, m_FrameDuration);
		}

		void Serialize(nlohmann::ordered_json& json) const override
		{
			nlohmann::ordered_json a;
			m_PositionInTexture.Serialize(a);
			json["Position"] = a;

			nlohmann::ordered_json b;
			m_FrameSize.Serialize(b);
			json["Frame Size"] = b;

			json["Frames Count"] = m_FramesCount;
			json["Frame Duration"] = m_FrameDuration;
		}

		bool Deserialize(std::vector<uint8_t>& byteArray, std::size_t& offset) override
		{
			const uint8_t version = Serializer::Deserialize<uint8_t>(byteArray, offset);
			if (version > ANIMATIONDATA_SERIALIZE_VERSION)
			{
				fmt::print(stderr, fg(fmt::color::red), "AnimationData::Deserialize() -> AnimationData has unsupported version {} (Current version is {}).\n", version, ANIMATIONDATA_SERIALIZE_VERSION);
				return false;
			}

			Vector2i pos;
			pos.Deserialize(byteArray, offset);
			m_PositionInTexture = pos;

			Vector2i size;
			size.Deserialize(byteArray, offset);
			m_FrameSize = size;

			m_FramesCount = Serializer::Deserialize<uint8_t>(byteArray, offset);
			m_FrameDuration = Serializer::Deserialize<float>(byteArray, offset);

			return true;
		}

		bool Deserialize(nlohmann::ordered_json& json) override
		{
			Vector2i pos;
			pos.Deserialize(json["Position"]);
			m_PositionInTexture = pos;

			Vector2i size;
			size.Deserialize(json["Frame Size"]);
			m_FrameSize = size;

			m_FramesCount = json.value("Frames Count", 0);
			m_FrameDuration = json.value("Frame Duration", 0.0f);

			return true;
		}

		//==== IPrintable ====

		const std::string& ToString() const override
		{
			nlohmann::ordered_json json;
			Serialize(json);
			return json.dump(2);
		}

		//==== Operators ====

		bool operator==(const AnimationData& other) const
		{
			return
				(	m_PositionInTexture == other.m_PositionInTexture
				&&	m_FrameSize			== other.m_FrameSize
				&&	m_FramesCount		== other.m_FramesCount
				&&	m_CurrentFrameIndex	== other.m_CurrentFrameIndex
				);
		}
	};
}