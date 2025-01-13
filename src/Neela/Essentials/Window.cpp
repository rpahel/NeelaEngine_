#include "Neela/Essentials/Window.h"
#include "Neela/ImGuiRenderer.h"

#include <stdexcept>
#include <iostream>

namespace Neela
{
	//=====================================================================================================
	//==== PUBLIC CONSTRUCTORS
	//=====================================================================================================

	Window::Window(const std::string& title, int width, int height, int x, int y, std::uint32_t flags) :
		m_SdlWindow(nullptr, SDL_DestroyWindow),
		m_Title(title),
		m_Renderer(nullptr)
	{
		try
		{
			m_SdlWindow.reset(SDL_CreateWindow(title.c_str(), x, y, width, height, flags));

			if (!m_SdlWindow)
				throw std::runtime_error("Window::Window() : Failed to create SdlWindow.");
		}
		catch (const std::exception& e)
		{
			printf("%s", e.what());
			printf("%s", SDL_GetError());
			exit(EXIT_FAILURE);
		}

		m_Renderer = std::make_shared<Renderer>(this);
		m_ImGuiRenderer = std::make_shared<ImGuiRenderer>(*this, m_Renderer);
	}

	Window::~Window()
	{
		SDL_DestroyWindow(m_SdlWindow.release());
	}

	//=====================================================================================================
	//==== PUBLIC METHODS
	//=====================================================================================================

	const std::string& Window::GetTitle() const
	{
		return m_Title;
	}

	std::string Window::GetDisplayTitle() const
	{
		return SDL_GetWindowTitle(m_SdlWindow.get());
	}

	void Window::SetDisplayTitle(const std::string& newTitle) const
	{
		SDL_SetWindowTitle(m_SdlWindow.get(), newTitle.c_str());
	}

	void Window::SetPosition(const Vector2i& newPos) const
	{
		SDL_SetWindowPosition(m_SdlWindow.get(), newPos.X, newPos.Y);
	}

	Vector2i Window::GetPosition() const
	{
		Vector2i ret;
		SDL_GetWindowPosition(m_SdlWindow.get(), &(ret.X), &(ret.Y));
		return ret;
	}

	int Window::GetWidth() const
	{
		int width = 0;
		int height = 0;
		SDL_GetWindowSize(m_SdlWindow.get(), &width, &height);
		return width;
	}

	int Window::GetHeight() const
	{
		int width = 0;
		int height = 0;
		SDL_GetWindowSize(m_SdlWindow.get(), &width, &height);
		return height;
	}

	Vector2i Window::GetSize() const
	{
		Vector2i size;
		SDL_GetWindowSize(m_SdlWindow.get(), &size.X, &size.Y);
		return size;
	}

	void Window::SetSize(const Vector2i& size)
	{
		SDL_SetWindowSize(m_SdlWindow.get(), size.X, size.Y);
	}

	uint32_t Window::GetFlags() const
	{
		return SDL_GetWindowFlags(m_SdlWindow.get());
	}

	Uint32 Window::GetID() const
	{
		return SDL_GetWindowID(m_SdlWindow.get());
	}

	std::weak_ptr<Renderer> Window::GetRenderer()
	{
		return m_Renderer;
	}

	void Window::SetBordered(bool bordered)
	{
		SDL_SetWindowBordered(m_SdlWindow.get(), static_cast<SDL_bool>(bordered));
	}

	const std::shared_ptr<ImGuiRenderer>& Window::GetImGuiRenderer() const
	{
		return m_ImGuiRenderer;
	}

	//=====================================================================================================
	//==== PUBLIC OPERATORS
	//=====================================================================================================

	Window::operator SDL_Window* ()
	{
		return m_SdlWindow.get();
	}
}