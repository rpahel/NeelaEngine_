#include "Neela/Systems/TimerSystem.h"
#include "Neela/Essentials/Timer.h"

#include <stdexcept>

namespace Neela
{
	TimerSystem* TimerSystem::m_Instance = nullptr;

	//====================================================================================
	//==== PUBLIC CONSTRUCTORS
	//====================================================================================

	TimerSystem::TimerSystem()
	{
		try
		{
			if (TimerSystem::m_Instance != nullptr)
				throw std::runtime_error("TimerSystem::TimerSystem() -> A TimerSystem has already been instanciated.");
		}
		catch (const std::exception& e)
		{
			printf("%s", e.what());
			exit(EXIT_FAILURE);
		}

		TimerSystem::m_Instance = this;
	}

	TimerSystem::~TimerSystem()
	{
		m_Instance = nullptr;
	}

	//====================================================================================
	//==== PUBLIC STATIC METHODS
	//====================================================================================

	std::weak_ptr<Timer> TimerSystem::CreateTimer(float timeToWait, const std::function<void()>& inCompletedAction, float inTimeOffset, bool loops)
	{
		EnsureInstance("TimerSystem::CreateTimer(float timeToWait, const std::function<void()>& inCompletedAction, float inTimeOffset, bool loops)");
	
		std::shared_ptr<Timer> ret = std::make_shared<Timer>(timeToWait, inCompletedAction, inTimeOffset, loops);
		TimerSystem::m_Instance->m_Timers.push_back(ret);
		return ret;
	}

	std::weak_ptr<Timer> TimerSystem::CreateContinuousTimer(float timerDuration, const std::function<void(float, float)>& inContinuousAction, float inTimeOffset, const std::function<void()>& inCompletedAction, bool loops)
	{
		EnsureInstance("TimerSystem::CreateContinuousTimer(float timeToWait, const std::function<void(float)>& inContinuousAction, const std::function<void()>& inCompletedAction, float inTimeOffset, bool loops)");

		std::shared_ptr<Timer> ret = std::make_shared<Timer>(timerDuration, inContinuousAction, inCompletedAction, inTimeOffset, loops);
		TimerSystem::m_Instance->m_Timers.push_back(ret);
		return ret;
	}

	void TimerSystem::UpdateTimers(float dTime)
	{
		EnsureInstance("UpdateTimers(double dTime)");

		const std::vector<std::shared_ptr<Timer>> timersToUpdate(TimerSystem::m_Instance->m_Timers);
		std::vector<Timer*> timersToKill;

		for (auto it = timersToUpdate.begin(); it != timersToUpdate.end(); ++it)
		{
			it->get()->Update(dTime);

			if (it->get()->m_IsCompleted)
			{
				timersToKill.emplace_back(it->get());
				continue;
			}
		}

		// GC the completed timers
		for (size_t i = 0; i < timersToKill.size(); ++i)
		{
			for (auto it = TimerSystem::m_Instance->m_Timers.begin(); it != TimerSystem::m_Instance->m_Timers.end();)
			{
				if (it->get() == timersToKill[i])
				{
					it = TimerSystem::m_Instance->m_Timers.erase(it);
					break;
				}
				++it;
			}
		}
	}

	//====================================================================================
	//==== PRIVATE STATIC METHODS
	//====================================================================================

	void TimerSystem::EnsureInstance(const std::string& functionName)
	{
		try
		{
			if (!TimerSystem::m_Instance)
				throw std::runtime_error("");
		}
		catch (const std::exception&)
		{
			printf("TimerSystem::%s -> No TimerSystem has been instanciated.\n", functionName.c_str());
			exit(EXIT_FAILURE);
		}
	}
}