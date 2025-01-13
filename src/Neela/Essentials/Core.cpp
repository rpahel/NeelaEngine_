#include "Neela/Essentials/Core.h"

#include <stdexcept>
#include <SDL2/SDL.h>
#include <iostream>

namespace Neela
{
	Core* Core::m_Instance;

	//=====================================================================================================
	//==== PUBLIC CONSTRUCTORS
	//=====================================================================================================

	Core::Core(uint32_t flags) :
		m_LastTime(SDL_GetPerformanceCounter())
	{
		try
		{
			if (Core::m_Instance)
				throw std::runtime_error("Core::Core(uint32_t flags) -> An instance of Core already exists.");
		}
		catch (const std::exception& e)
		{
			printf("%s", e.what());
			exit(2);
		}

		SDL_Init(flags);
		Core::m_Instance = this;
	}

	Core::~Core()
	{
		SDL_Quit();
		Core::m_Instance = nullptr;
	}

	//=====================================================================================================
	//==== PUBLIC STATIC METHODS
	//=====================================================================================================

	double Core::EnsureFrameRate()
	{
		EnsureInstance("EnsureFrameRate()");

		SDL_Delay(static_cast<Uint32>(Core::m_Instance->m_FrameDuration));

		const std::uint64_t now = SDL_GetPerformanceCounter();
		const double elapsed_time = static_cast<double>(now - Core::m_Instance->m_LastTime) / SDL_GetPerformanceFrequency();
		Core::m_Instance->m_DeltaTime =
			Core::m_Instance->m_IsPaused
			? 0
			: elapsed_time * Core::m_Instance->m_TimeScale;

		Core::m_Instance->m_ProgramTime += elapsed_time;
		
		Core::m_Instance->m_LastTime = now;
		Core::m_Instance->m_GameTime += Core::m_Instance->m_DeltaTime;

		return Core::m_Instance->m_DeltaTime;
	}

	void Core::SetFrameRate(int32_t frameRate)
	{
		EnsureInstance("SetFrameRate(unsigned int frameRate)");

		if (frameRate <= 0)
		{
			printf("Core::SetFrameRate(unsigned int frameRate) -> Can't set frame rate to lower or equal than 0 fps. Setting it to 1...");
			frameRate = 1;
		}

		Core::m_Instance->m_FrameDuration = 1000.0 / frameRate;
	}

	unsigned int Core::GetFrameRate()
	{
		EnsureInstance("GetFrameRate()");
		return Core::m_Instance->m_DeltaTime == 0 ? 0 : (int)(1.0 / Core::m_Instance->m_DeltaTime);
	}

	void Core::SetPaused(bool isPaused)
	{
		EnsureInstance("SetIsPaused()");
		Core::m_Instance->m_IsPaused = isPaused;
	}

	bool Core::IsPaused()
	{
		EnsureInstance("IsPaused()");
		return Core::m_Instance->m_IsPaused;
	}

	void Core::SetTimeScale(double newTimeScale)
	{
		EnsureInstance("SetTimeScale(double newTimeScale)");
		Core::m_Instance->m_TimeScale = newTimeScale;
	}

	double Core::GetTimeScale()
	{
		EnsureInstance("GetTimeScale()");
		return Core::m_Instance->m_TimeScale;
	}

	entt::registry& Core::World()
	{
		EnsureInstance("World()");
		return Core::m_Instance->m_World;
	}

	double Core::DeltaTime()
	{
		EnsureInstance("DeltaTime()");
		return Core::m_Instance->m_DeltaTime;
	}

	double Core::GameTime()
	{
		EnsureInstance("GameTime()");
		return Core::m_Instance->m_GameTime;
	}

	double Core::ProgramTime()
	{
		EnsureInstance("ProgramTime()");
		return Core::m_Instance->m_ProgramTime;
	}

	//=============================================================================================
	//==== PRIVATE STATIC METHODS	
	//=============================================================================================

	void Core::EnsureInstance(const std::string& functionName)
	{
		try
		{
			if (!Core::m_Instance)
				throw std::runtime_error("");
		}
		catch (const std::exception&)
		{
			printf("Core::%s -> No Core has been instanciated.\n", functionName.c_str());
			exit(EXIT_FAILURE);
		}
	}
}
