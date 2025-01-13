#include "Neela/Systems/AudioSystem.h"
#include "Neela/NeelaConstants.h"

#include <stdexcept>
#include <AL/al.h>
#include <AL/alc.h>

namespace Neela
{
	AudioSystem* AudioSystem::m_Instance = nullptr;
	float AudioSystem::MasterVolume = 1;

	//====================================================================================
	//==== PUBLIC CONSTRUCTORS
	//====================================================================================

	AudioSystem::AudioSystem()
	{
		try
		{
			if (AudioSystem::m_Instance != nullptr)
				throw std::runtime_error("AudioSystem::AudioSystem() -> A AudioSystem has already been instanciated.");
		}
		catch (const std::exception& e)
		{
			printf("%s", e.what());
			exit(EXIT_FAILURE);
		}

		AudioSystem::m_Instance = this;
		AudioSystem::MasterVolume = 1;
		m_AudioDevice = alcOpenDevice(AUDIO_DEVICE);
		m_AudioContext = alcCreateContext(m_AudioDevice, nullptr);
		alcMakeContextCurrent(m_AudioContext);
	}

	AudioSystem::~AudioSystem()
	{
		m_Instance = nullptr;
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(m_AudioContext);
		alcCloseDevice(m_AudioDevice);
	}

	//====================================================================================
	//==== PUBLIC STATIC METHODS
	//====================================================================================

	void AudioSystem::SetMasterVolume(float newVolume)
	{
		AudioSystem::MasterVolume = newVolume;
	}

	float AudioSystem::GetMasterVolume()
	{
		return AudioSystem::MasterVolume;
	}

	//====================================================================================
	//==== PRIVATE STATIC METHODS
	//====================================================================================

	void AudioSystem::EnsureInstance(const std::string& functionName)
	{
		try
		{
			if (!AudioSystem::m_Instance)
				throw std::runtime_error("");
		}
		catch (const std::exception&)
		{
			printf("AudioSystem::%s -> No AudioSystem has been instanciated.\n", functionName.c_str());
			exit(EXIT_FAILURE);
		}
	}
}