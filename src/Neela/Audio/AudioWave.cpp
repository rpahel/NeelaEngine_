#include "Neela/Audio/AudioWave.h"
#include "Neela/Essentials/ResourcesManager.h"
#include "Neela/Essentials/Serializer.h"

#include "AL/al.h"
#include "AL/alc.h"
#include "fmt/core.h"
#include "fmt/color.h"

#define AUDIOWAVE_SERIALIZE_VERSION 1

namespace Neela
{
	//=============================================================================================
	//==== PUBLIC CONSTRUCTORS
	//=============================================================================================

	AudioWave::AudioWave(const std::string& path, const std::vector<std::int16_t>& samples) :
		m_Path(path),
		m_Samples(samples)
	{
	}

	//=============================================================================================
	//==== PUBLIC METHODS
	//=============================================================================================

	const std::vector<std::int16_t>& AudioWave::GetSamples() const
	{
		return m_Samples;
	}

	void AudioWave::SetSamples(const std::vector<std::int16_t>& samples)
	{
		m_Samples = samples;
	}

	const std::string& AudioWave::GetPath() const
	{
		return m_Path;
	}

	void AudioWave::SetPath(const std::string& inPath)
	{
		m_Path = inPath;
	}

	void AudioWave::SetVolume(float volume)
	{
		m_Volume = volume;
	}

	float AudioWave::GetVolume() const
	{
		return m_Volume;
	}

	void AudioWave::SetPitch(float pitch)
	{
		m_Pitch = pitch;
	}

	float AudioWave::GetPitch() const
	{
		return m_Pitch;
	}

	void AudioWave::Play(ALuint source, bool resume)
	{
		if (!resume)
			alSourceRewind(source);

		alSourcePlay(source);
	}

	void AudioWave::Pause(ALuint source)
	{
		alSourcePause(source);
	}

	void AudioWave::Stop(ALuint source)
	{
		alSourceStop(source);
	}

	//=============================================================================================
	//==== ISERIALIZEABLE
	//=============================================================================================

	void AudioWave::Serialize(std::vector<uint8_t>& byteArray) const
	{
		Serializer::Serialize<uint8_t>(byteArray, AUDIOWAVE_SERIALIZE_VERSION);
		Serializer::Serialize(byteArray, GetPath());
		Serializer::Serialize<float>(byteArray, GetVolume());
		Serializer::Serialize<float>(byteArray, GetPitch());
	}

	void AudioWave::Serialize(nlohmann::ordered_json& j) const
	{
		if(!m_Path.empty())
			j["Path"] = GetPath();

		if(m_Volume != 1)
			j["Volume"]	= GetVolume();

		if(m_Pitch != 1)
			j["Pitch"]	= GetPitch();
	}

	//=============================================================================================
	//==== IFILESERIALIZEABLE
	//=============================================================================================

	bool AudioWave::SaveToFileJson(const std::filesystem::path& filepath) const
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
			fmt::print(fg(fmt::color::red), "AudioWave::SaveToFileJson() -> Failed to open AudioWave file {}.\n", filepath.string());
			return false;
		}

		nlohmann::ordered_json waveDoc;
		Serialize(waveDoc);

		file << waveDoc.dump(2);
		file.close();

		fmt::print(fg(fmt::color::lime_green), "AudioWave::SaveToFileJson() -> Saved AudioWave to {}.\n", filepath.string());
		return true;
	}

	bool AudioWave::SaveToFileBin(const std::filesystem::path& filepath) const
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
			fmt::print(fg(fmt::color::red), "AudioWave::SaveToFileBin() -> Failed to open AudioWave file {}.\n", filepath.string());
			return false;
		}

		std::vector<std::uint8_t> buffer;
		Serialize(buffer);
		file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());

		file.close();

		fmt::print(fg(fmt::color::lime_green), "AudioWave::SaveToFileBin() -> Saved AudioWave to {}.\n", filepath.string());
		return true;
	}

	bool AudioWave::Deserialize(std::vector<uint8_t>& byteArray, std::size_t& offset)
	{
		const uint8_t version = Serializer::Deserialize<uint8_t>(byteArray, offset);
		if (version > AUDIOWAVE_SERIALIZE_VERSION)
		{
			fmt::print(stderr, fg(fmt::color::red), "AudioWave::Deserialize() -> AudioWave has unsupported version {} (Current version is {}).\n", version, AUDIOWAVE_SERIALIZE_VERSION);
			return false;
		}

		SetPath(Serializer::Deserialize<std::string>(byteArray, offset));
		SetVolume(Serializer::Deserialize<float>(byteArray, offset));
		SetPitch(Serializer::Deserialize<float>(byteArray, offset));

		return true;
	}

	bool AudioWave::Deserialize(nlohmann::ordered_json& json)
	{
		SetPath(json.value("Path", ""));
		SetVolume(json.value("Volume", 1.0f));
		SetPitch(json.value("Pitch", 1.0f));

		return true;
	}

	//=============================================================================================
	//==== IPRINTABLE
	//=============================================================================================

	const std::string& AudioWave::ToString() const
	{
		nlohmann::ordered_json waveDoc;
		Serialize(waveDoc);
		return fmt::format("{}", waveDoc.dump(2));
	}
}