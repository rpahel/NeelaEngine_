#include "Neela/NeelaCore.h"
#include "Common/sv_GameServer.h"

#include "Game/sh_NetworkManager.h"

int main(int argc, char* argv[])
{
	// Initialisation des systemes.
	Neela::Core c;
	Neela::ResourcesManager rm;
	Neela::TimerSystem ts;
	ACNet::NetworkManager networkManager;

	ACNet::GameServer gameServer;
	gameServer.StartServer();

	while (gameServer.IsRunning())
	{
		Neela::Core::EnsureFrameRate();
		gameServer.HandleTick();
	}

	return EXIT_SUCCESS;
}

#ifdef NEELA_ENGINE_STATIC
#include <windows.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	return main(__argc, __argv);
}
#endif