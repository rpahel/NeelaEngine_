#pragma once

#include "Neela/Graphics/SpriteSheet.h"
#include "Neela/Graphics/AnimationData.h"
#include "Neela/Maths/Rect.h"

#include <optional>
#include <cassert>
#include <nlohmann/json.hpp>

#define SPRITESHEET_SERIALIZE_VERSION 1

namespace Neela
{
	//====================================================================================
	//==== PUBLIC CONSTRUCTORS
	//====================================================================================

	SpriteSheet::SpriteSheet(const std::unordered_map<std::string, AnimationData>& animationsMap)
	{
		m_CurrentAnimation = nullptr;
		m_AnimationsMap = animationsMap;

		if (m_AnimationsMap.size() != 0)
			m_CurrentAnimation = &(m_AnimationsMap.begin()->second);
	}

	//====================================================================================
	//==== PUBLIC METHODS
	//====================================================================================

	std::unordered_map<std::string, AnimationData> SpriteSheet::GetAnimationsMap() const
	{
		return m_AnimationsMap;
	}

	void SpriteSheet::SetAnimationsMap(const std::unordered_map<std::string, AnimationData>& newMap)
	{
		m_AnimationsMap = newMap;
		m_CurrentAnimation = m_AnimationsMap.size() <= 0 ? nullptr : &(m_AnimationsMap.begin()->second);
	}

	void SpriteSheet::Clear()
	{
		m_AnimationsMap.clear();
		m_CurrentAnimation = nullptr;
	}

	void SpriteSheet::PlayAnimation(const std::string& animationName, bool restartAnimation)
	{
		assert(m_AnimationsMap.size() > 0);

		auto it = m_AnimationsMap.find(animationName);

		assert(it != m_AnimationsMap.end());

		if (m_CurrentAnimation != &(it->second) || restartAnimation)
		{
			m_CurrentAnimation = &(it->second);
			m_CurrentAnimation->m_CurrentFrameIndex = 0;
		}
	}

	void SpriteSheet::AddAnimationData(const std::string& name, const AnimationData& animData)
	{
		if (m_AnimationsMap.size() != 0 && m_AnimationsMap.contains(name))
			return;

		std::optional<AnimationData> tmp = m_CurrentAnimation ? std::optional<AnimationData>(*m_CurrentAnimation) : std::nullopt;

		auto result = m_AnimationsMap.emplace(name, animData);

		if (!tmp.has_value())
			m_CurrentAnimation = &(result.first->second);
		else
		{
			for (auto it = m_AnimationsMap.begin(); it != m_AnimationsMap.end(); it++)
			{
				if (tmp.value() == it->second)
				{
					m_CurrentAnimation = &(it->second);
				}
			}
		}

		assert(m_CurrentAnimation);
	}

	void SpriteSheet::AddAnimationData(const std::vector<std::tuple<const char*, AnimationData>>& animations)
	{
		for (const std::tuple<const char*, AnimationData>& anim : animations)
			AddAnimationData(std::get<0>(anim), std::get<1>(anim));
	}

	Rect SpriteSheet::GetSourceRectByTime(float time)
	{
		assert(m_CurrentAnimation);
		return GetSourceRectByIndex(static_cast<int>((time * m_AnimationSpeedFactor) / m_CurrentAnimation->m_FrameDuration) % m_CurrentAnimation->m_FramesCount);
	}

	Rect SpriteSheet::GetSourceRectByIndex(int frameIndex)
	{
		assert(m_CurrentAnimation);
		m_CurrentAnimation->m_CurrentFrameIndex = frameIndex % m_CurrentAnimation->m_FramesCount;
		return m_CurrentAnimation->GetCurrentFrameRect();
	}

	Rect SpriteSheet::GetSourceRect(const std::string& animationName, int frameIndex)
	{
		assert(m_AnimationsMap.size() > 0);

		auto it = m_AnimationsMap.find(animationName);

		assert(it != m_AnimationsMap.end());

		AnimationData& animData = it->second;
		m_CurrentAnimation = &animData;
		m_CurrentAnimation->m_CurrentFrameIndex = frameIndex % m_CurrentAnimation->m_FramesCount;
		return m_CurrentAnimation->GetCurrentFrameRect();
	}

	void SpriteSheet::SetAnimationSpeedFactor(float factor)
	{
		m_AnimationSpeedFactor = factor;
	}

	bool SpriteSheet::HasCurrentAnimation()
	{
		return m_CurrentAnimation;
	}

	//====================================================================================
	//==== IPRINTABLE
	//====================================================================================

	const std::string& SpriteSheet::ToString() const
	{
		nlohmann::ordered_json json;
		Serialize(json);
		return json.dump(2);
	}

	//====================================================================================
	//==== IFILESERIALIZEABLE
	//====================================================================================

	void SpriteSheet::Serialize(std::vector<uint8_t>& byteArray) const
	{
		// Version
		Serializer::Serialize<uint8_t>(byteArray, SPRITESHEET_SERIALIZE_VERSION);

		// Speed factor
		Serializer::Serialize<float>(byteArray, m_AnimationSpeedFactor);

		// AnimationData number
		Serializer::Serialize<uint8_t>(byteArray, m_AnimationsMap.size());

		// Animations
		for (auto& element : m_AnimationsMap)
		{
			Serializer::Serialize(byteArray, element.first);
			element.second.Serialize(byteArray);
		}
	}

	void SpriteSheet::Serialize(nlohmann::ordered_json& json) const
	{
		json["Speed Factor"] = m_AnimationSpeedFactor;

		nlohmann::ordered_json k;
		for (auto& element : m_AnimationsMap)
		{
			nlohmann::ordered_json l;
			element.second.Serialize(l);
			k.push_back({ element.first, l });
		}

		json["Animations Map"] = k;
	}

	bool SpriteSheet::SaveToFileJson(const std::filesystem::path& filepath) const
	{
		if (!std::filesystem::exists(filepath))
		{
			std::filesystem::path folders(filepath);
			folders.remove_filename();
			try
			{
				std::filesystem::create_directories(folders);
			}
			catch (std::filesystem::filesystem_error& e)
			{
				fmt::print(fg(fmt::color::red), "{}\n", e.what());
				return false;
			}
		}

		std::ofstream file(filepath);
		if (!file.is_open())
		{
			fmt::print(fg(fmt::color::red), "SpriteSheet::SaveToFileJson() -> Failed to open SpriteSheet file {}.\n", filepath.string());
			return false;
		}

		nlohmann::ordered_json json;
		Serialize(json);

		file << json.dump(2);
		file.close();

		fmt::print(fg(fmt::color::lime_green), "SpriteSheet::SaveToFileJson() -> Saved SpriteSheet to {}.\n", filepath.string());
		return true;
	}

	bool SpriteSheet::SaveToFileBin(const std::filesystem::path& filepath) const
	{
		if (!std::filesystem::exists(filepath))
		{
			std::filesystem::path folders(filepath);
			folders.remove_filename();
			try
			{
				std::filesystem::create_directories(folders);
			}
			catch (std::filesystem::filesystem_error& e)
			{
				fmt::print(fg(fmt::color::red), "{}\n", e.what());
				return false;
			}
		}

		std::ofstream file(filepath, std::ios::out | std::ios::binary);
		if (!file.is_open())
		{
			fmt::print(fg(fmt::color::red), "SpriteSheet::SaveToFileBin() -> Failed to open SpriteSheet file {}.\n", filepath.string());
			return false;
		}

		std::vector<std::uint8_t> buffer;
		Serialize(buffer);
		file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());

		file.close();

		fmt::print(fg(fmt::color::lime_green), "SpriteSheet::SaveToFileBin() -> Saved SpriteSheet to {}.\n", filepath.string());
		return true;
	}

	bool SpriteSheet::Deserialize(std::vector<uint8_t>& byteArray, std::size_t& offset)
	{
		const uint8_t version = Serializer::Deserialize<uint8_t>(byteArray, offset);
		if (version > SPRITESHEET_SERIALIZE_VERSION)
		{
			fmt::print(stderr, fg(fmt::color::red), "SpriteSheet::Deserialize() -> SpriteSheet has unsupported version {} (Current version is {}).\n", version, SPRITESHEET_SERIALIZE_VERSION);
			return false;
		}

		const float speedFactor = Serializer::Deserialize<float>(byteArray, offset);
		SetAnimationSpeedFactor(speedFactor);

		const uint8_t animNb = Serializer::Deserialize<uint8_t>(byteArray, offset);
		for (size_t i = 0; i < animNb; i++)
		{
			const std::string name = Serializer::Deserialize<std::string>(byteArray, offset);
			AnimationData animData;
			animData.Deserialize(byteArray, offset);
			AddAnimationData(name, animData);
		}

		return true;
	}

	bool SpriteSheet::Deserialize(nlohmann::ordered_json& json)
	{
		const float speedFactor = json.value("Speed Factor", 0.0f);
		SetAnimationSpeedFactor(speedFactor);

		for (auto& j : json.at("Animations Map"))
		{
			AnimationData animData;
			animData.Deserialize(j[1]);
			AddAnimationData(j[0], animData);
		}

		return true;
	}

	//====================================================================================
	//==== IINSPECTABLE
	//====================================================================================

	void SpriteSheet::PopulateInspector()
	{
		if (!ImGui::TreeNode("Sprite sheet"))
			return;

		ImGui::Text("Number of animations : %u", m_AnimationsMap.size());

		std::string current_anim = "None";

		if (m_CurrentAnimation)
		{
			for (auto& i : m_AnimationsMap)
			{
				if (i.second == *m_CurrentAnimation)
				{
					current_anim = i.first;
					break;
				}
			}
		}

		const char* anim_name = current_anim.c_str();

		if (ImGui::BeginCombo("Animation", anim_name))
		{
			for (std::size_t i = 0; i < m_AnimationsMap.size(); ++i)
			{
				auto it = m_AnimationsMap.begin();
				for (size_t j = 0; j < i; j++, it++){};

				const std::string key = it->first;
				bool isSelected = (key == current_anim);
				if (ImGui::Selectable(key.c_str()))
					PlayAnimation(key);

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		if(m_CurrentAnimation)
			m_CurrentAnimation->PopulateInspector();

		float speed = m_AnimationSpeedFactor;
		if(ImGui::SliderFloat("Animation speed", &speed, 0, 3.0f, "%.2f"))
			SetAnimationSpeedFactor(speed);

		ImGui::TreePop();
	}
}