#pragma once

#include "Neela/Essentials/Export.h"

#include <memory>
#include <functional>

namespace Neela
{
	class NEELA_ENGINE_API Timer
	{
		friend class TimerSystem;

	private:
		bool m_IsPaused = false;
		bool m_IsCompleted = false;
		bool m_Loops = false;
		float m_TimeOffset = 0;
		float m_TimeToWait = 0;
		float m_CurrentTime = 0;

		std::function<void()> m_CompletedAction;
		std::function<void(float, float)> m_ContinuousAction;

	public:
		//==== Constructors ====

		// Please don't. Use Neela::TimerSystem::CreateTimer instead.
		explicit Timer(float timeToWait, const std::function<void()>& inCompletedAction = std::function<void()>(), float inTimeOffset = 0, bool loops = false);

		// Please don't. Use Neela::TimerSystem::CreateTimer instead.
		explicit Timer(float timeToWait, const std::function<void(float, float)>& inContinuousAction = std::function<void(float, float)>(), const std::function<void()>& inCompletedAction = std::function<void()>(), float inTimeOffset = 0, bool loops = false);

		//==== Methods ====

		void Play();
		void Pause();
		void Reset(bool executeFunctor = false);
		void Stop(bool executeFunctor = false);
		void SetIsLooping(bool isLooping);

	private:
		//==== Methods ====

		void Update(float dTime);
		void CallContinuousAction(float dTime);
	};
}