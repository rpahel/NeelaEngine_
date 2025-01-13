#include "Neela/NeelaCore.h"
#include "Common/cl_GameClient.h"

#include <enet6/enet.h>
#include <fmt/core.h>

#include "Game/cl_ACGameInstance.h"
#include "Game/sh_NetworkManager.h"

int main(int argc, char* argv[])
{
	// Initialisation des systemes.
	Neela::Core c;
	Neela::WindowsManager wm;
	Neela::ResourcesManager rm;
	Neela::InputsManager im;
	Neela::TimerSystem ts;
	Neela::AudioSystem as;
	ACNet::NetworkManager networkManager;

	ACGame::Game game;
	game.Initialize();

	ACGame::ACGameInstance gameInstance;
	gameInstance.Initialize();

	while (gameInstance.IsRunning() && Neela::WindowsManager::IsMainWindowOpen())
	{
		Neela::Core::EnsureFrameRate();
		gameInstance.HandleTick();
	}

	gameInstance.Shutdown();
	game.Shutdown();

	return EXIT_SUCCESS;
}

#ifdef NEELA_ENGINE_STATIC
#include <windows.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	return main(__argc, __argv);
}
#endif