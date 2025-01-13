#pragma once

#include "Neela/Essentials/Export.h"

#include <string>

struct ALCdevice;
struct ALCcontext;

namespace Neela
{
	class NEELA_ENGINE_API AudioSystem
	{
	private:
		static AudioSystem* m_Instance;
		static float MasterVolume;

		ALCdevice* m_AudioDevice;
		ALCcontext* m_AudioContext;

	public:
		//==== Constructors ====

		AudioSystem();
		AudioSystem(const AudioSystem&) = delete;
		AudioSystem(AudioSystem&&) = delete;
		~AudioSystem();

		//==== Static Methods ====

		static void SetMasterVolume(float newVolume);
		static float GetMasterVolume();

		//==== Operators ====

		AudioSystem& operator=(const AudioSystem&) = delete;
		AudioSystem& operator=(AudioSystem&&) = delete;

	private:
		//==== Static Methods ====

		static void EnsureInstance(const std::string& functionName);
	};
}