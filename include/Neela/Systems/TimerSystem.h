#pragma once

#include "Neela/Essentials/Export.h"

#include <memory>
#include <vector>
#include <functional>
#include <string>

namespace Neela
{
	class Timer;

	class NEELA_ENGINE_API TimerSystem
	{
	private:
		static TimerSystem* m_Instance;
		std::vector<std::shared_ptr<Timer>> m_Timers;

	public:
		//==== Constructors ====

		TimerSystem();
		TimerSystem(const TimerSystem&) = delete;
		TimerSystem(TimerSystem&&) = delete;
		~TimerSystem();

		//==== Static Methods ====

		static std::weak_ptr<Timer> CreateTimer(float timeToWait, const std::function<void()>& inCompletedAction, float inTimeOffset = 0, bool loops = false);
		static std::weak_ptr<Timer> CreateContinuousTimer(float timerDuration, const std::function<void(float, float)>& inContinuousAction, float inTimeOffset = 0, const std::function<void()>& inCompletedAction = std::function<void()>(), bool loops = false);
		static void UpdateTimers(float dTime);

		//==== Operators ====

		TimerSystem& operator=(const TimerSystem&) = delete;
		TimerSystem& operator=(TimerSystem&&) = delete;

	private:
		//==== Static Methods ====

		static void EnsureInstance(const std::string& functionName);
	};
}