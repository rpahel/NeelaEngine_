#include "Neela/Components/AudioComponent.h"
#include "Neela/Systems/AudioSystem.h"

#include <fmt/core.h>
#include <fmt/color.h>

namespace Neela
{
	//=============================================================================================
	//==== PUBLIC CONSTRUCTORS
	//=============================================================================================

	AudioComponent::AudioComponent()
	{
		m_Source = 0;
		alGenSources(1, &m_Source);
	}

	AudioComponent::~AudioComponent()
	{
		for (auto& i : m_AudioWavesMap)
			Stop(i.first);

		alDeleteSources(1, &m_Source);
	}

	//=============================================================================================
	//==== PUBLIC METHODS
	//=============================================================================================

	void AudioComponent::Play(const std::string& audioName, bool resume) const
	{
		if (m_AudioWavesMap.size() <= 0)
		{
			fmt::print(fg(fmt::color::red), "AudioComponent::Play() -> No sounds registered !\n");
			return;
		}

		AudioWave* audioToPlay;
		auto it = m_AudioWavesMap.find(audioName);

		if (audioName != "" && it == m_AudioWavesMap.end())
		{
			fmt::print(fg(fmt::color::red), "AudioComponent::Play() -> No sound named {} was registered in this component.\n", audioName);
			return;
		}

		audioToPlay = audioName == "" ? m_AudioWavesMap.begin()->second.get() : it->second.get();

		if (!audioToPlay)
		{
			fmt::print(fg(fmt::color::red), "AudioComponent::Play() -> audioToPlay is null.\n");
			return;
		}

		ALuint buffer;
		alGenBuffers(1, &buffer);
		alBufferData(buffer, AL_FORMAT_STEREO16, audioToPlay->GetSamples().data(), audioToPlay->GetSamples().size() * sizeof(std::int16_t), 44100);
		alSourcei(m_Source, AL_BUFFER, buffer);

		alSourcef(m_Source, AL_GAIN, audioToPlay->GetVolume() * AudioSystem::GetMasterVolume());
		audioToPlay->Play(m_Source, resume);
	}

	void AudioComponent::Pause(const std::string& audioName) const
	{
		if (m_AudioWavesMap.size() <= 0)
		{
			fmt::print(fg(fmt::color::red), "AudioComponent::Pause() -> No sounds registered !\n");
			return;
		}

		AudioWave* audioToPause;
		auto it = m_AudioWavesMap.find(audioName);

		if (audioName != "" && it == m_AudioWavesMap.end())
		{
			fmt::print(fg(fmt::color::red), "AudioComponent::Pause() -> No sound named {} was registered in this component.\n", audioName);
			return;
		}

		audioToPause = it->second.get();

		if (!audioToPause)
		{
			fmt::print(fg(fmt::color::red), "AudioComponent::Pause() -> audioToPause is null.\n");
			return;
		}

		audioToPause->Pause(m_Source);
	}

	void AudioComponent::Stop(const std::string& audioName) const
	{
		if (m_AudioWavesMap.size() <= 0)
		{
			fmt::print(fg(fmt::color::red), "AudioComponent::Stop() -> No sounds registered !\n");
			return;
		}

		AudioWave* audioToStop;
		auto it = m_AudioWavesMap.find(audioName);

		if (audioName != "" && it == m_AudioWavesMap.end())
		{
			fmt::print(fg(fmt::color::red), "AudioComponent::Stop() -> No sound named {} was registered in this component.\n", audioName);
			return;
		}

		audioToStop = it->second.get();

		if (!audioToStop)
		{
			fmt::print(fg(fmt::color::red), "AudioComponent::Stop() -> audioToStop is null.\n");
			return;
		}

		audioToStop->Stop(m_Source);
	}

	const std::unordered_map<std::string, std::shared_ptr<AudioWave>>& AudioComponent::GetAudioWavesMap() const
	{
		return m_AudioWavesMap;
	}

	void AudioComponent::ClearAudioWaveMap()
	{
		m_AudioWavesMap.clear();
	}

	void AudioComponent::AddAudioWave(const std::string& audioName, const std::shared_ptr<AudioWave>& audioWave)
	{
		if (audioName.size() == 0)
		{
			fmt::print(fg(fmt::color::red), "AudioComponent::AddAudioWave() -> audioName is empty.\n");
			return;
		}

		if (m_AudioWavesMap.size() >= 255)
		{
			fmt::print(fg(fmt::color::red), "AudioComponent::AddAudioWave() -> m_AudioWavesMap size limit reached.\n");
			return;
		}

		m_AudioWavesMap.emplace(audioName, audioWave);
	}

	void AudioComponent::RemoveAudioWave(const std::string& audioName)
	{
		if (m_AudioWavesMap.size() == 0)
		{
			fmt::print(fg(fmt::color::yellow), "AudioComponent::RemoveAudioWave() -> m_AudioWavesMap is empty.\n");
			return;
		}

		if(audioName.size() == 0)
			return;

		auto it = m_AudioWavesMap.find(audioName);
		if (it == m_AudioWavesMap.end())
		{
			fmt::print(fg(fmt::color::yellow), "AudioComponent::RemoveAudioWave() -> Could not find AudioWave named {0} in this AudioComponent.\n", audioName);
			return;
		}

		m_AudioWavesMap.erase(it);
	}

	void AudioComponent::SetAudioWaveVolume(float newVolume, const std::string& audioName)
	{
		if (m_AudioWavesMap.size() == 0)
		{
			fmt::print(fg(fmt::color::yellow), "AudioComponent::SetAudioWaveVolume() -> m_AudioWavesMap is empty.\n");
			return;
		}

		if (audioName.size() == 0)
		{
			fmt::print(fg(fmt::color::yellow), "AudioComponent::SetAudioWaveVolume() -> audioName is empty.\n");
			return;
		}

		auto it = m_AudioWavesMap.find(audioName);
		if (it == m_AudioWavesMap.end())
		{
			fmt::print(fg(fmt::color::yellow), "AudioComponent::SetAudioWaveVolume() -> Could not find AudioWave named {0} in this AudioComponent.\n", audioName);
			return;
		}

		if (it->second)
		{
			alSourcef(m_Source, AL_GAIN, newVolume * AudioSystem::GetMasterVolume());
			it->second->SetVolume(newVolume);
		}
	}

	float AudioComponent::GetAudioWaveVolume(const std::string& audioName) const
	{
		if (m_AudioWavesMap.size() == 0)
		{
			fmt::print(fg(fmt::color::yellow), "AudioComponent::GetAudioWaveVolume() -> m_AudioWavesMap is empty.\n");
			return -1;
		}

		if (audioName.size() == 0)
		{
			fmt::print(fg(fmt::color::yellow), "AudioComponent::GetAudioWaveVolume() -> audioName is empty.\n");
			return -1;
		}

		auto it = m_AudioWavesMap.find(audioName);
		if (it == m_AudioWavesMap.end())
		{
			fmt::print(fg(fmt::color::yellow), "AudioComponent::GetAudioWaveVolume() -> Could not find AudioWave named {0} in this AudioComponent.\n", audioName);
			return -1;
		}

		return it->second ? it->second->GetVolume() : -1;
	}

	//=============================================================================================
	//====	ISERIALIZEABLE
	//=============================================================================================

	//void AudioComponent::SerializeBin(std::vector<uint8_t>& byteArray) const
	//{
	//	// Version
	//	Serializer::SerializeBinary<uint8_t>(byteArray, Serializer::GetSerializeVersion_AudioComponent());

	//	// Map size (reserve)
	//	std::size_t sizeOffset = Serializer::SerializeBinary<uint8_t>(byteArray, 0);

	//	uint8_t size = 0;
	//	// List of audio name - audio wave
	//	for (auto& pair : m_AudioWavesMap)
	//	{
	//		if(!pair.second)
	//			return;

	//		Serializer::SerializeBinary_String(byteArray, pair.first);
	//		pair.second->SerializeBin(byteArray);
	//		size++;
	//	}

	//	// Map size
	//	Serializer::SerializeBinary<uint8_t>(byteArray, size, sizeOffset);
	//}

	//void AudioComponent::SerializeJson(nlohmann::ordered_json& j) const
	//{
	//	j["Size"] = 0;

	//	nlohmann::json k;
	//	uint8_t size = 0;
	//	for (auto& pair : m_AudioWavesMap)
	//	{
	//		if(!pair.second)
	//			return;

	//		k["Name"] = pair.first;

	//		nlohmann::ordered_json l;
	//		pair.second->SerializeJson(l);
	//		k["AudioWave"] = l;

	//		j["AudioWaves"].emplace_back(k);

	//		size++;
	//	}

	//	j["Size"] = size;
	//}
}
