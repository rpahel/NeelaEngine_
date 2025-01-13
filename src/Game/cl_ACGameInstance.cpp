#include "Game/cl_ACGameInstance.h"

#include "Neela/Essentials/WindowsManager.h"
#include "Neela/Essentials/Window.h"
#include "Neela/ImGuiRenderer.h"
#include "Neela/Essentials/Core.h"
#include "Neela/Essentials/InputsManager.h"
#include "Neela/Systems/AnimationSystem.h"
#include "Neela/Systems/InterfaceSystem.h"
#include "Neela/Systems/RenderSystem.h"
#include "Neela/Systems/TimerSystem.h"

namespace ACGame
{
    ACGameInstance::ACGameInstance() :
    m_state(State::None),
    m_hosting(false)
    {
    }

    ACGame::ACGameInstance::~ACGameInstance()
    {
    }

    bool ACGameInstance::IsRunning() const
    {
        return m_state != State::Exit;
    }

    void ACGameInstance::Initialize()
    {
        ImGui::SetCurrentContext(Neela::WindowsManager::GetMainWindow().lock()->GetImGuiRenderer()->GetContext());

        SwitchState(State::MainMenu);
    }
    
    void ACGameInstance::HandleTick()
    {
        switch (m_state)
        {
        case State::None:
            throw;
        case State::MainMenu:
            {
                MainMenuStateTick();
                break;
            }
        case State::Game:
            {
                GameStateTick();
                break;
            }
        default:
            throw;
        }

        // Base systems
        Neela::AnimationSystem::UpdateAnimations(Neela::Core::World());

        Neela::RenderSystem::Clear(Neela::Core::World());
        Neela::InterfaceSystem::DrawInterfaces(Neela::Core::World());
        Neela::RenderSystem::Draw(Neela::Core::World());
        Neela::RenderSystem::Present(Neela::Core::World());

        Neela::InputsManager::PollEvents();
        Neela::TimerSystem::UpdateTimers(Neela::Core::DeltaTime());
        //
    }

    void ACGameInstance::Shutdown()
    {
        SwitchState(State::Exit); // will abort state switch if already in exit state
    }

    void ACGameInstance::SwitchState(State newState)
    {
        if(newState == m_state)
            return;

        switch (m_state)
        {
        case State::None:
            break;
        case State::MainMenu:
            InternalExitMainMenuState();
            break;
        case State::Game:
            InternalExitGameState();
            break;
        }

        m_state = newState;
        switch (m_state)
        {
        case State::None:
            throw;
        case State::MainMenu:
            InternalEnterMainMenuState();
            break;
        case State::Game:
            InternalEnterGameState();
            break;
        case State::Exit:
            if(Neela::WindowsManager::IsMainWindowOpen())
                Neela::WindowsManager::CloseMainWindow();
            break;
        }
    }

    void ACGameInstance::InternalEnterMainMenuState()
    {
        m_state = State::MainMenu;
        m_mainMenu = std::make_unique<MainMenu>();
        m_mainMenu->Start();
    }

    void ACGameInstance::MainMenuStateTick()
    {
        m_mainMenu->Update();

        if(m_mainMenu->GetState() == MainMenu::State::GameStarted)
            SwitchState(State::Game);
        else if(m_mainMenu->GetState() == MainMenu::State::Quitted)
            SwitchState(State::Exit);
    }
    
    void ACGameInstance::InternalExitMainMenuState()
    {
        if(m_mainMenu)
        {
            // Collected info of main menu
            m_targetIpAddress = m_mainMenu->GetIP();
            m_playerUsername = m_mainMenu->GetUsername();
            m_characterAppearance = m_mainMenu->GetCharacterAppearance();
            m_hosting = m_mainMenu->GetShouldHost();
            
            // Stop main menu
            m_mainMenu->Stop();
        }

        m_mainMenu.reset();
    }

    void ACGameInstance::InternalEnterGameState()
    {
        m_gameClient = std::make_unique<ACNet::GameClient>();
        m_gameClient->Initialize();
        
        if(m_hosting)
        {
            m_gameServer = std::make_unique<ACNet::GameServer>();
            m_gameServer->StartServer();
        }
    }

    void ACGameInstance::GameStateTick()
    {
        if(m_gameClient)
        {
            if(m_gameClient->IsRunning())
            {
                if(m_gameClient->GetState() == ACNet::GameClient::State::Offline)
                    m_gameClient->RequestServerConnection(m_targetIpAddress);
                else if (m_gameClient->GetState() == ACNet::GameClient::State::ConnectedToServer)
                    m_gameClient->RequestGameJoin(m_playerUsername, m_characterAppearance);

                m_gameClient->HandleTick();
            }
            else
                SwitchState(State::MainMenu);
        }

        if(m_gameServer)
        {
            if(m_gameServer->IsRunning())
                m_gameServer->HandleTick();
            else
                SwitchState(State::MainMenu);
        }
    }

    void ACGameInstance::InternalExitGameState()
    {
        m_gameClient.reset();
        m_gameServer.reset();
    }
}
