#include "Neela/ImGuiRenderer.h"
#include "Neela/Essentials/Renderer.h"
#include "Neela/Essentials/Window.h"

#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>

namespace Neela
{
	//====================================================================================
	//==== PUBLIC CONSTRUCTORS
	//====================================================================================

	ImGuiRenderer::ImGuiRenderer(Window& window, const std::weak_ptr<Renderer>& renderer)
	{
		// Setup imgui
		IMGUI_CHECKVERSION();
		m_Context = ImGui::CreateContext();
		m_Renderer = renderer;

		ImGui::StyleColorsDark();

		ImGui_ImplSDL2_InitForSDLRenderer(window, *renderer.lock());
		ImGui_ImplSDLRenderer2_Init(*renderer.lock());
	}

	ImGuiRenderer::~ImGuiRenderer()
	{
		ImGui_ImplSDLRenderer2_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext(m_Context);
	}

	//====================================================================================
	//==== PUBLIC METHODS
	//====================================================================================

	ImGuiContext* ImGuiRenderer::GetContext()
	{
		return m_Context;
	}

	void ImGuiRenderer::Render()
	{
		ImGui::Render();
		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), *m_Renderer.lock());
	}

	void ImGuiRenderer::NewFrame()
	{
		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiRenderer::ProcessEvent(SDL_Event& event)
	{
		ImGui_ImplSDL2_ProcessEvent(&event);
	}
}