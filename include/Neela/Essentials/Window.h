#pragma once

#include "Neela/Essentials/Export.h"
#include "Neela/Essentials/Renderer.h"
#include "Neela/Maths/Vector.h"

#include <SDL2/SDL.h>
#include <memory>
#include <string>

namespace Neela
{
	class Renderer;
	class ImGuiRenderer;

	class NEELA_ENGINE_API Window
	{
		friend class WindowsManager;

	private:
		std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> m_SdlWindow;
		std::string m_Title;
		std::shared_ptr<Renderer> m_Renderer;
		std::shared_ptr<ImGuiRenderer> m_ImGuiRenderer;

	public:
		//==== Constructors ====

		Window(const std::string& title, int width, int height, int x, int y, std::uint32_t flags);
		Window(const Window&) = delete;
		Window(Window&&) = delete;
		~Window();

		//==== Methods ====

		const std::string& GetTitle() const;
		std::string GetDisplayTitle() const;
		void SetDisplayTitle(const std::string& newTitle) const;

		void SetPosition(const Vector2i& newPos) const;
		Vector2i GetPosition() const;

		int GetWidth() const;
		int GetHeight() const;
		Vector2i GetSize() const;
		void SetSize(const Vector2i& size);

		uint32_t GetFlags() const;
		Uint32 GetID() const;

		std::weak_ptr<Renderer> GetRenderer();

		void SetBordered(bool bordered);

		const std::shared_ptr<ImGuiRenderer>& GetImGuiRenderer() const;

		//==== Operators ====

		Window& operator=(const Window&) = delete;
		Window& operator=(Window&&) = delete;
		operator SDL_Window* ();
	};
}