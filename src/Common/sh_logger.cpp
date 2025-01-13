#include "Common/sh_logger.h" 

namespace ACNet
{
	fmt::color ACLogger::GetColor(LogSource source, LogType type)
	{
		switch(type)
		{
		case LogType::None:
			ACLog(source, LogType::Warning, "Should not use the 'None' value of LogType");
			return fmt::color::white;
		case LogType::Error:
			return fmt::color::red;
		case LogType::Warning:
			return fmt::color::orange;
		case LogType::Debug:
			return fmt::color::gold;
		case LogType::InfoNetwork:
			return fmt::color::cyan;
		case LogType::InfoGameState:
			return fmt::color::lime_green;
		case LogType::InfoMoney:
			return fmt::color::gold;
		case LogType::InfoOthers:
			return fmt::color::green;
		default:
			throw;
		}
	}

	const char* ACLogger::GetSourceFormat(LogSource source)
	{
		switch(source)
		{
		case LogSource::None:
		case LogSource::Common:
			return "";
		case LogSource::Application:
			return "[APPLICATION] ";
		case LogSource::Server:
			return "[SERVER] ";
		case LogSource::Client:
			return "[CLIENT] ";
		default: throw;
		}
	}
}
