#include "Game/sh_NetworkManager.h"
#include <enet6/enet.h>
#include <stdexcept>
#include <fmt/color.h>

#include "Common/sh_logger.h"

namespace ACNet
{
	NetworkManager::NetworkManager()
	{
		if(enet_initialize() != 0)
		{
			ACLogError(LogSource::Application, "Failed to initialize enet.\n");
			throw std::runtime_error("Failed to initialize enet.");
		}
	}

	NetworkManager::~NetworkManager()
	{
		enet_deinitialize();
	}
}
