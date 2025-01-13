#pragma once

#include "Neela/Essentials/Export.h"
#include "Neela/Interfaces/ISerializeable.h"
#include "Neela/Audio/AudioWave.h"

#include <memory>
#include <unordered_map>
#include <AL/al.h>
#include <AL/alc.h>

namespace Neela
{
	struct NEELA_ENGINE_API AudioComponent
	{
	private:
		//==== Properties

		std::unordered_map<std::string, std::shared_ptr<AudioWave>> m_AudioWavesMap;
		ALuint m_Source;

	public:
		//==== Constructors

		AudioComponent();
		~AudioComponent();

		//==== Methods

		void Play(const std::string& audioName = "", bool resume = false) const;
		void Pause(const std::string& audioName = "") const;
		void Stop(const std::string& audioName = "") const;

		const std::unordered_map<std::string, std::shared_ptr<AudioWave>>& GetAudioWavesMap() const;
		void ClearAudioWaveMap();

		void AddAudioWave(const std::string& audioName, const std::shared_ptr<AudioWave>& audioWave);
		void RemoveAudioWave(const std::string& audioName);

		void SetAudioWaveVolume(float newVolume, const std::string& audioName = "");
		float GetAudioWaveVolume(const std::string& audioName = "") const;
	};
}