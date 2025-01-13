#pragma once

#include "Neela/Essentials/Export.h"
#include "Neela/Interfaces/IFileSerializeable.h"
#include "Neela/Interfaces/IPrintable.h"

#include <string>
#include <vector>
#include <AL/al.h>
#include <AL/alc.h>

namespace Neela
{
	class NEELA_ENGINE_API AudioWave : public IFileSerializeable, public IPrintable
	{
	private:
		float m_Volume = 1;
		float m_Pitch = 1;
		std::string m_Path = "";
		std::vector<std::int16_t> m_Samples;

	public:
		//==== Constructors ====
		
		AudioWave() = default;
		AudioWave(const std::string& path, const std::vector<std::int16_t>& samples);

		//==== Methods ====

		const std::vector<std::int16_t>& GetSamples() const;
		void SetSamples(const std::vector<std::int16_t>& samples);

		const std::string& GetPath() const;
		void SetPath(const std::string& inPath);

		void SetVolume(float volume);
		float GetVolume() const;

		void SetPitch(float pitch);
		float GetPitch() const;

		void Play(ALuint source, bool resume = false);
		void Pause(ALuint source);
		void Stop(ALuint source);

		//==== ISerializeable ====

		void Serialize(std::vector<uint8_t>& byteArray) const override;
		void Serialize(nlohmann::ordered_json& j) const override;

		//==== IFileSerializeable ====

		bool SaveToFileJson(const std::filesystem::path& filepath) const override;
		bool SaveToFileBin(const std::filesystem::path& filepath) const override;

		bool Deserialize(std::vector<uint8_t>& byteArray, std::size_t& offset) override;
		bool Deserialize(nlohmann::ordered_json& json) override;

		//==== IPrintable ====

		const std::string& ToString() const override;
	};
}