#include <catch2/catch_all.hpp>
#include <Common/sh_logger.h>

TEST_CASE("ACLog macro")
{
	ACLog(ACNet::LogSource::Server, ACNet::LogType::Error, "Error");
	ACLog(ACNet::LogSource::Server, ACNet::LogType::Warning, "Warning");
	ACLog(ACNet::LogSource::Server, ACNet::LogType::Debug, "Debug");
	ACLog(ACNet::LogSource::Server, ACNet::LogType::InfoGameState, "InfoGameState");
	ACLog(ACNet::LogSource::Server, ACNet::LogType::InfoNetwork, "InfoNetwork");
	ACLog(ACNet::LogSource::Server, ACNet::LogType::InfoOthers, "InfoOthers");

	ACLog(ACNet::LogSource::Client, ACNet::LogType::Error, "Error");
	ACLog(ACNet::LogSource::Client, ACNet::LogType::Warning, "Warning");
	ACLog(ACNet::LogSource::Client, ACNet::LogType::Debug, "Debug");
	ACLog(ACNet::LogSource::Client, ACNet::LogType::InfoGameState, "InfoGameState");
	ACLog(ACNet::LogSource::Client, ACNet::LogType::InfoNetwork, "InfoNetwork");
	ACLog(ACNet::LogSource::Client, ACNet::LogType::InfoOthers, "InfoOthers");

	ACLog(ACNet::LogSource::Application, ACNet::LogType::Error, "Error");
	ACLog(ACNet::LogSource::Application, ACNet::LogType::Warning, "Warning");
	ACLog(ACNet::LogSource::Application, ACNet::LogType::Debug, "Debug");
	ACLog(ACNet::LogSource::Application, ACNet::LogType::InfoGameState, "InfoGameState");
	ACLog(ACNet::LogSource::Application, ACNet::LogType::InfoNetwork, "InfoNetwork");
	ACLog(ACNet::LogSource::Application, ACNet::LogType::InfoOthers, "InfoOthers");
}

TEST_CASE("ACLog macro with parameters")
{
	ACLog(ACNet::LogSource::Server, ACNet::LogType::Error, "Error {0}, {1}", 153, "test");
	ACLog(ACNet::LogSource::Server, ACNet::LogType::Warning, "Warning {0}, {1}", 153, "test");
	ACLog(ACNet::LogSource::Server, ACNet::LogType::Debug, "Debug {0}, {1}", 153, "test");
	ACLog(ACNet::LogSource::Server, ACNet::LogType::InfoGameState, "InfoGameState {0}, {1}", 153, "test");
	ACLog(ACNet::LogSource::Server, ACNet::LogType::InfoNetwork, "InfoNetwork {0}, {1}", 153, "test");
	ACLog(ACNet::LogSource::Server, ACNet::LogType::InfoOthers, "InfoOthers {0}, {1}", 153, "test");

	ACLog(ACNet::LogSource::Client, ACNet::LogType::Error, "Error {0}, {1}", 153, "test");
	ACLog(ACNet::LogSource::Client, ACNet::LogType::Warning, "Warning {0}, {1}", 153, "test");
	ACLog(ACNet::LogSource::Client, ACNet::LogType::Debug, "Debug {0}, {1}", 153, "test");
	ACLog(ACNet::LogSource::Client, ACNet::LogType::InfoGameState, "InfoGameState {0}, {1}", 153, "test");
	ACLog(ACNet::LogSource::Client, ACNet::LogType::InfoNetwork, "InfoNetwork {0}, {1}", 153, "test");
	ACLog(ACNet::LogSource::Client, ACNet::LogType::InfoOthers, "InfoOthers {0}, {1}", 153, "test");

	ACLog(ACNet::LogSource::Application, ACNet::LogType::Error, "Error {0}, {1}", 153, "test");
	ACLog(ACNet::LogSource::Application, ACNet::LogType::Warning, "Warning {0}, {1}", 153, "test");
	ACLog(ACNet::LogSource::Application, ACNet::LogType::Debug, "Debug {0}, {1}", 153, "test");
	ACLog(ACNet::LogSource::Application, ACNet::LogType::InfoGameState, "InfoGameState {0}, {1}", 153, "test");
	ACLog(ACNet::LogSource::Application, ACNet::LogType::InfoNetwork, "InfoNetwork {0}, {1}", 153, "test");
	ACLog(ACNet::LogSource::Application, ACNet::LogType::InfoOthers, "InfoOthers {0}, {1}", 153, "test");
}

TEST_CASE("ACLogError macro")
{
	ACLogError(ACNet::LogSource::Server, "Error");
	ACLogError(ACNet::LogSource::Application, "Error");
	ACLogError(ACNet::LogSource::Application, "Error");
}

TEST_CASE("ACLogWarning macro")
{
	ACLogWarning(ACNet::LogSource::Server, "Warning");
	ACLogWarning(ACNet::LogSource::Client, "Warning");
	ACLogWarning(ACNet::LogSource::Application, "Warning");
}