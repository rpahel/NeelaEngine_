#include "Neela/Essentials/WindowsManager.h"
#include "Neela/Essentials/Window.h"

#include <stdexcept>
#include <iostream>
#include <cassert>

namespace Neela
{
	WindowsManager* WindowsManager::m_Instance;

	//=====================================================================================================
	//==== PUBLIC CONSTRUCTORS
	//=====================================================================================================

	WindowsManager::WindowsManager()
	{
		try
		{
			if (WindowsManager::m_Instance)
				throw std::runtime_error("WindowsManager::WindowsManager() -> An instance of WindowsManager already exists.");
		}
		catch (const std::exception& e)
		{
			printf("%s", e.what());
			exit(2);
		}

		WindowsManager::m_Instance = this;
		m_MainWindow.reset();
	}

	WindowsManager::~WindowsManager()
	{
		DestroyAllWindows();
	}

	//=====================================================================================================
	//==== PUBLIC STATIC METHODS
	//=====================================================================================================

	std::weak_ptr<Window> WindowsManager::InstantiateWindow(const std::string& windowTitle, int width, int height, int x, int y, std::uint32_t flags)
	{
		EnsureInstance("CreateWindow(const std::string& title, int width, int height, bool makeMainWindow, int x, int y, std::uint32_t flags)");

		assert(windowTitle != "");

		auto it = WindowsManager::m_Instance->m_WindowsMap.find(windowTitle);
		if (it != WindowsManager::m_Instance->m_WindowsMap.end())
		{
			printf("WindowsManager::CreateWindow(const std::string& title, int width, int height, bool makeMainWindow, int x, int y, std::uint32_t flags) -> A Window titled \"%s\" already exists.", windowTitle.c_str());
			return std::weak_ptr<Window>();
		}

		std::shared_ptr<Window> s_ptr = std::make_shared<Window>(windowTitle, width, height, x, y, flags);
		auto pair = WindowsManager::m_Instance->m_WindowsMap.emplace(windowTitle, s_ptr);

		if (!pair.second)
		{
			printf("WindowsManager::CreateWindow(const std::string& title, int width, int height, bool makeMainWindow, int x, int y, std::uint32_t flags) -> A Window titled \"%s\" could not be created.", windowTitle.c_str());
			return std::weak_ptr<Window>();
		}

		if (WindowsManager::m_Instance->m_WindowsMap.size() == 1)
			SetMainWindow(s_ptr.get());

		return s_ptr;
	}

	void WindowsManager::SetMainWindow(Window* window)
	{
		EnsureInstance("SetMainWindow(Window* window)");

		if (!window)
		{
			printf("WindowsManager::SetMainWindow(Window* window) -> window is nullptr.");
			return;
		}

		const std::string& title = window->GetTitle();
		auto it = WindowsManager::m_Instance->m_WindowsMap.find(window->GetTitle());
		if (it == WindowsManager::m_Instance->m_WindowsMap.end())
		{
			printf("WindowsManager::SetMainWindow(Window* window) -> Could not find window titled %s in WindowsManager.", title.c_str());
			return;
		}

		WindowsManager::m_Instance->m_MainWindow = it->second;
	}

	bool WindowsManager::IsMainWindowOpen()
	{
		EnsureInstance("IsMainWindowOpen()");
		return !WindowsManager::m_Instance->m_MainWindow.expired();
	}

	std::weak_ptr<Window> WindowsManager::GetMainWindow()
	{
		EnsureInstance("GetMainWindow()");
		return WindowsManager::m_Instance->m_MainWindow;
	}

	std::weak_ptr<Window> WindowsManager::GetWindowByTitle(const std::string& title, EGetWindowBehaviour behaviour)
	{
		EnsureInstance("GetWindowByTitle(const std::string& title)");

		auto it = WindowsManager::m_Instance->m_WindowsMap.find(title);
		if (it == WindowsManager::m_Instance->m_WindowsMap.end())
		{
			printf("WindowsManager::GetWindowByTitle(const std::string& title) -> No Window titled \"%s\" found.", title.c_str());
			return std::weak_ptr<Window>();
		}

		return it->second;
	}

	std::weak_ptr<Window> WindowsManager::GetWindowByID(Uint32 id, EGetWindowBehaviour behaviour)
	{
		EnsureInstance("GetWindowByID(Uint32 id)");

		auto begin = WindowsManager::m_Instance->m_WindowsMap.begin();
		auto end = WindowsManager::m_Instance->m_WindowsMap.end();
		for (auto it = begin; it != end; ++it)
		{
			if (it->second->GetID() == id)
				return it->second;
		}

		printf("WindowsManager::GetWindowByID(Uint32 id) -> No Window with id \"%i\" found.", id);
		return std::weak_ptr<Window>();
	}

	const std::unordered_map<std::string, std::shared_ptr<Window>>& WindowsManager::GetWindows()
	{
		EnsureInstance("GetWindows()");
		return  WindowsManager::m_Instance->m_WindowsMap;
	}

	void WindowsManager::CloseMainWindow()
	{
		EnsureInstance("CloseMainWindow()");
		CloseWindow(WindowsManager::m_Instance->m_MainWindow.lock().get());
	}

	void WindowsManager::CloseWindow(Window* window)
	{
		EnsureInstance("CloseWindow(Window* window)");

		if (!window)
		{
			printf("WindowsManager::CloseWindow(Window* window) -> window is nullptr.");
			return;
		}

		if (window == WindowsManager::m_Instance->m_MainWindow.lock().get())
			WindowsManager::m_Instance->m_MainWindow.reset();

		auto begin = WindowsManager::m_Instance->m_WindowsMap.begin();
		auto end = WindowsManager::m_Instance->m_WindowsMap.end();
		for (auto it = begin; it != end; ++it)
		{
			if (it->second.get() == window)
			{
				WindowsManager::m_Instance->m_WindowsMap.erase(it);
				return;
			}
		}

		printf("WindowsManager::CloseWindow(Window* window) -> The provided window is not managed by WindowsManager.");
	}

	void WindowsManager::CloseWindowByTitle(const std::string& title)
	{
		EnsureInstance("CloseWindowByTitle(const std::string& title)");

		if (title == WindowsManager::m_Instance->m_MainWindow.lock()->GetTitle())
		{
			CloseMainWindow();
			return;
		}

		size_t count = WindowsManager::m_Instance->m_WindowsMap.erase(title);

		if (count == 0)
			printf("WindowsManager::CloseWindowByTitle(const std::string& title) -> Could not erase window titled %s.", title.c_str());
	}

	void WindowsManager::CloseWindowByID(Uint32 id)
	{
		EnsureInstance("CloseWindowByID(Uint32 id)");
		CloseWindow(GetWindowByID(id).lock().get());
	}

	//=====================================================================================================
	//==== PRIVATE METHODS
	//=====================================================================================================

	void WindowsManager::DestroyAllWindows()
	{
		m_WindowsMap.clear();
	}

	void WindowsManager::EnsureInstance(const std::string& functionName)
	{
		try
		{
			if (WindowsManager::m_Instance == nullptr)
				throw std::runtime_error("");
		}
		catch (const std::exception&)
		{
			printf("WindowsManager::%s -> No Instance of WindowsManager found.\n", functionName.c_str());
			exit(2);
		}
	}
}
