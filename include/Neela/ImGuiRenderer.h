#pragma once

#include "Neela/Essentials/Export.h"

#include <memory>

union SDL_Event;
struct ImGuiContext;

namespace Neela
{
	class Renderer;
	class Window;

	class NEELA_ENGINE_API ImGuiRenderer
	{
	private:
		ImGuiContext* m_Context;
		std::weak_ptr<Renderer> m_Renderer;

	public:
		//==== Constructors ===

		ImGuiRenderer(Window& window, const std::weak_ptr<Renderer>& renderer);
		ImGuiRenderer(const ImGuiRenderer&) = delete;
		ImGuiRenderer(ImGuiRenderer&&) = delete;
		~ImGuiRenderer();

		//==== Methods ===

		ImGuiContext* GetContext();
		void Render();
		void NewFrame();
		void ProcessEvent(SDL_Event& event);

		//==== Operators ===

		ImGuiRenderer& operator=(const ImGuiRenderer&) = delete;
		ImGuiRenderer& operator=(ImGuiRenderer&&) = delete;
	};
}
