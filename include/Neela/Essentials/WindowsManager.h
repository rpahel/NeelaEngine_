#pragma once

#include "Neela/Essentials/Export.h"
#include "Neela/Enums/GetWindowBehaviourEnum.h"

#include <SDL2/SDL.h>
#include <memory>
#include <unordered_map>
#include <string>

namespace Neela
{
	class Window;

	class NEELA_ENGINE_API WindowsManager
	{
	private:
		static WindowsManager* m_Instance;
		std::unordered_map<std::string, std::shared_ptr<Window>> m_WindowsMap;
		std::weak_ptr<Window> m_MainWindow;

	public:
		//==== Constructors ====

		WindowsManager();
		WindowsManager(const WindowsManager&) = delete;
		WindowsManager(WindowsManager&&) = delete;
		~WindowsManager();

		//==== Static Methods ====

		/* Named InstantiateWindow instead of a CreateWindow because of a define in WinUser.h */
		static std::weak_ptr<Window> InstantiateWindow(const std::string& windowTitle, int width, int height, int x = SDL_WINDOWPOS_CENTERED, int y = SDL_WINDOWPOS_CENTERED, std::uint32_t flags = 0);

		// Passing nullptr to SetMainWindow() will close the app.
		static void SetMainWindow(Window* window);
		static bool IsMainWindowOpen();
		static std::weak_ptr<Window> GetMainWindow();
		static std::weak_ptr<Window> GetWindowByTitle(const std::string& title, EGetWindowBehaviour behaviour = EGetWindowBehaviour::THROW_ERROR);
		static std::weak_ptr<Window> GetWindowByID(Uint32 id, EGetWindowBehaviour behaviour = EGetWindowBehaviour::THROW_ERROR);
		static const std::unordered_map<std::string, std::shared_ptr<Window>>& GetWindows();

		// Closing the main window will close the app.
		static void CloseMainWindow();
		static void CloseWindow(Window* window);
		static void CloseWindowByTitle(const std::string& title);
		static void CloseWindowByID(Uint32 id);

		//==== Operators ====

		WindowsManager& operator=(const WindowsManager&) = delete;
		WindowsManager& operator=(WindowsManager&&) = delete;

	private:
		//==== Methods ====

		void DestroyAllWindows();
		static void EnsureInstance(const std::string& functionName);
	};
}