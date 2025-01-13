#pragma once
#include <memory>

#include "Common/cl_GameClient.h"
#include "Common/sv_GameServer.h"
#include "Common/MainMenu/cl_MainMenu.h"

namespace ACGame
{
    class ACGameInstance
    {
    private:
        enum class State : std::uint8_t
        {
            None = 0,
            MainMenu,
            Game,
            Exit
        };
        State m_state;
        
        std::unique_ptr<MainMenu> m_mainMenu;
        
        std::unique_ptr<ACNet::GameServer> m_gameServer;
        std::unique_ptr<ACNet::GameClient> m_gameClient;

        bool m_hosting;
        std::string m_targetIpAddress;
        CharacterAppearance m_characterAppearance;
        std::string m_playerUsername;
        
    public:
        ACGameInstance();
        ~ACGameInstance();

        bool IsRunning() const;
        
        void Initialize();
        void HandleTick();
        void Shutdown();

    private:
        void SwitchState(State newState);
        
        void InternalEnterMainMenuState();
        void MainMenuStateTick();
        void InternalExitMainMenuState();
        
        void InternalEnterGameState();
        void GameStateTick();
        void InternalExitGameState();
    };
}
