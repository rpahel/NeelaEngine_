#pragma once 
#include <string>
#include <fmt/color.h>

namespace ACNet
{
	enum class LogSource
	{
		None = 0,
		Application,
		Server,
		Client,
		Common
	};

	enum class LogType
	{
		None = 0,
		Error, // Something not normal happening
		Warning, // Something not normal happening but does not crash the game
		Debug, // Temporary logs for debugging
		InfoNetwork, // General logs informing of messages received/sent, and network events
		InfoGameState, // General logs informing of various game events, states and changes
		InfoMoney, // General logs informing of various money events and changes
		InfoOthers // General logs not falling in the above categories
	};

	class ACLogger
	{
	public:
		static fmt::color GetColor(LogSource source, LogType type);
		static const char* GetSourceFormat(LogSource source);
	};
}

#ifdef AC_DISTRIB
#define ACLog(Source, Type, format, ...) //
#define ACLogError(Source, format, ...) //
#define ACLogWarning(Source, format, ...) //

#else

#define ACLog(Source, Type, format, ...) \
		fmt::print(fg(ACNet::ACLogger::GetColor(Source, Type)), fmt::runtime(ACNet::ACLogger::GetSourceFormat(Source))); \
		fmt::print(fg(ACNet::ACLogger::GetColor(Source, Type)), format, __VA_ARGS__)

#define ACLogError(Source, format, ...) \
		fmt::print(stderr, fg(ACNet::ACLogger::GetColor(Source, ACNet::LogType::Error)), fmt::runtime(ACNet::ACLogger::GetSourceFormat(Source))); \
		fmt::print(stderr, fg(ACNet::ACLogger::GetColor(Source, ACNet::LogType::Error)), format, __VA_ARGS__)

#define ACLogWarning(Source, format, ...) \
		fmt::print(stderr, fg(ACNet::ACLogger::GetColor(Source, ACNet::LogType::Warning)), fmt::runtime(ACNet::ACLogger::GetSourceFormat(Source))); \
		fmt::print(stderr, fg(ACNet::ACLogger::GetColor(Source, ACNet::LogType::Warning)), format, __VA_ARGS__)
#endif