#include "Neela/Essentials/Timer.h"

namespace Neela
{
	//====================================================================================
	//==== PUBLIC CONSTRUCTORS
	//====================================================================================

	Timer::Timer(float timeToWait, const std::function<void()>& inCompletedAction, float inTimeOffset, bool loops) :
		Timer(timeToWait, std::function<void(float, float)>(), inCompletedAction, inTimeOffset, loops)
	{
	}

	Timer::Timer(float timeToWait, const std::function<void(float, float)>& inContinuousAction, const std::function<void()>& inCompletedAction, float inTimeOffset, bool loops) :
		m_IsPaused(false),
		m_IsCompleted(false),
		m_Loops(loops),
		m_TimeOffset(inTimeOffset),
		m_TimeToWait(timeToWait),
		m_CurrentTime(0),
		m_ContinuousAction(inContinuousAction),
		m_CompletedAction(inCompletedAction)
	{
	}

	//====================================================================================
	//==== PUBLIC METHODS
	//====================================================================================

	void Timer::Play()
	{
		m_IsPaused = false;
	}

	void Timer::Pause()
	{
		m_IsPaused = true;
	}

	void Timer::Reset(bool executeFunctor)
	{
		if (executeFunctor && m_CompletedAction)
			m_CompletedAction();

		m_CurrentTime = 0;
	}

	void Timer::Stop(bool executeFunctor)
	{
		if(executeFunctor && m_CompletedAction)
			m_CompletedAction();

		m_IsCompleted = true;
	}

	void Timer::SetIsLooping(bool isLooping)
	{
		m_Loops = isLooping;
	}

	//====================================================================================
	//==== PRIVATE METHODS
	//====================================================================================

	void Timer::Update(float dTime)
	{
		if(m_IsCompleted || m_IsPaused)
			return;

		if (m_TimeOffset > 0)
		{
			m_TimeOffset -= dTime;
			return;
		}
		else
		{
			m_CurrentTime += dTime;
		}

		if (m_CurrentTime < m_TimeToWait)
		{
			CallContinuousAction(dTime);
			return;
		}

		if (m_Loops)
		{
			Reset(true);
		}
		else
		{
			Stop(true);
		}
	}
	void Timer::CallContinuousAction(float dTime)
	{
		if(m_ContinuousAction)
			m_ContinuousAction(dTime, m_CurrentTime);
	}
}