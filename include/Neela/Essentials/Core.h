#pragma once

#include "Neela/Essentials/Export.h"
#include "Neela/NeelaConstants.h"

#include <cstdint>
#include <string>
#include <entt/entt.hpp>

namespace Neela
{
	class NEELA_ENGINE_API Core
	{
	private:
		static Core* m_Instance;
		entt::registry m_World;
		bool m_IsPaused = false;

		double m_TimeScale = 1;
		double m_FrameDuration = DEFAULT_FRAME_DURATION;
		double m_DeltaTime = 0;
		double m_ProgramTime = 0; // Unaffected by pause
		double m_GameTime = 0; // Affected by pause
		std::uint64_t m_LastTime = 0;

	public:
		//==== Constructors ====

		Core(uint32_t flags = 0);
		Core(const Core&) = delete;
		Core(Core&&) = delete;
		~Core();

		//==== Static Methods ====

		static double EnsureFrameRate();
		static void SetFrameRate(int32_t frameRate);
		static unsigned int GetFrameRate();
		static void SetPaused(bool isPaused);
		static bool IsPaused();
		static void SetTimeScale(double newTimeScale);
		static double GetTimeScale();
		static entt::registry& World();
		static double DeltaTime();
		static double GameTime();
		static double ProgramTime();

		//==== Operators ====

		Core& operator=(const Core&) = delete;
		Core& operator=(Core&&) = delete;

	private:
		//==== Static Methods ====

		static void EnsureInstance(const std::string& functionName);
	};
}