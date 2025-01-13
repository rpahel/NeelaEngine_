#pragma once

#include "Neela/Essentials/Export.h"
#include "Neela/Graphics/AnimationData.h"
#include "Neela/Interfaces/IPrintable.h"
#include "Neela/Interfaces/IFileSerializeable.h"
#include "Neela/Interfaces/IInspectable.h"

#include <unordered_map>
#include <tuple>

namespace Neela
{
	struct Rect;

	class NEELA_ENGINE_API SpriteSheet : public IPrintable, public IFileSerializeable, public IInspectable
	{
	private:
		std::unordered_map<std::string, AnimationData> m_AnimationsMap;
		AnimationData* m_CurrentAnimation = nullptr;
		float m_AnimationSpeedFactor = 1;

	public:
		//==== Constructors ====

		SpriteSheet() = default;
		SpriteSheet(const std::unordered_map<std::string, AnimationData>& animationsMap);

		//==== Methods ====

		std::unordered_map<std::string, AnimationData> GetAnimationsMap() const;
		void SetAnimationsMap(const std::unordered_map<std::string, AnimationData>& newMap);
		void Clear();

		void PlayAnimation(const std::string& animationName, bool restartAnimation = false);
		void AddAnimationData(const std::string& name, const AnimationData& animData);
		void AddAnimationData(const std::vector<std::tuple<const char*, AnimationData>>& animations);

		Rect GetSourceRectByTime(float time);
		Rect GetSourceRectByIndex(int frameIndex);
		Rect GetSourceRect(const std::string& animationName, int frameIndex = 0);

		void SetAnimationSpeedFactor(float factor);

		bool HasCurrentAnimation();

		//==== IPrintable ====

		const std::string& ToString() const override;

		//==== IFileSerializeable ====

		void Serialize(std::vector<uint8_t>& byteArray) const override;
		void Serialize(nlohmann::ordered_json& json) const override;

		bool SaveToFileJson(const std::filesystem::path& filepath) const override;
		bool SaveToFileBin(const std::filesystem::path& filepath) const override;

		bool Deserialize(std::vector<uint8_t>& byteArray, std::size_t& offset) override;
		bool Deserialize(nlohmann::ordered_json& json) override;

		//==== IInspectable ====

		void PopulateInspector() override;
	};
}