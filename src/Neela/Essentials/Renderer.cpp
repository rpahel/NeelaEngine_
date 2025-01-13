#include "Neela/Essentials/Renderer.h"
#include "Neela/Essentials/Window.h"
#include "Neela/Graphics/Texture.h"
#include "Neela/Graphics/Color.h"

#include <stdexcept>

namespace Neela
{
	//=====================================================================================================
	//==== CONSTRUCTORS
	//=====================================================================================================

	Renderer::Renderer(Window* window, int index, uint32_t flags) :
		m_SdlRenderer(nullptr, SDL_DestroyRenderer)
	{
		try
		{
			if (!window)
				throw std::runtime_error("Renderer::Renderer() : window is nullptr.");

			m_SdlRenderer.reset(SDL_CreateRenderer(*window, index, flags));

			if (!m_SdlRenderer)
				throw std::runtime_error("Renderer::Renderer() : Failed to create SdlRenderer.");
		}
		catch (const std::exception& e)
		{
			printf("%s", e.what());
			printf("%s", SDL_GetError());
			exit(2);
		}
	}

	Renderer::Renderer(Renderer&& victim) noexcept :
		m_SdlRenderer(nullptr, SDL_DestroyRenderer)
	{
		m_SdlRenderer.reset(victim.m_SdlRenderer.release());
	}

	Renderer::~Renderer()
	{
		if (m_SdlRenderer.get())
			SDL_DestroyRenderer(m_SdlRenderer.release());
	}

	//=====================================================================================================
	//==== PUBLIC METHODS
	//=====================================================================================================

	void Renderer::Clear() const
	{
		SDL_RenderClear(m_SdlRenderer.get());
	}

	void Renderer::Present() const
	{
		SDL_RenderPresent(m_SdlRenderer.get());
	}

	void Renderer::SetBackgroundColor(const Color& newColor) const
	{
		SDL_SetRenderDrawColor(m_SdlRenderer.get(), newColor.R, newColor.G, newColor.B, newColor.A);
	}

	//=====================================================================================================
	//==== OPERATORS
	//=====================================================================================================

	Renderer& Renderer::operator=(Renderer&& victim) noexcept
	{
		std::swap(m_SdlRenderer, victim.m_SdlRenderer);
		return *this;
	}

	Renderer::operator SDL_Renderer* ()
	{
		return m_SdlRenderer.get();
	}

	//=====================================================================================================
	//==== PRIVATE METHODS
	//=====================================================================================================

	void Renderer::DrawTexture(const Texture* texture, Rect* destinationRect, Rect* sourceRect)
	{
		if (!texture)
			return;

		SDL_Rect sdlDestinationRect{};
		SDL_Rect sdlSourceRect{};

		if (destinationRect)
			sdlDestinationRect = destinationRect->GetSdlRect();

		if (sourceRect)
			sdlSourceRect = sourceRect->GetSdlRect();

		SDL_RenderCopy(
			*this,
			texture->GetSdlTexture(),
			sourceRect ? &sdlSourceRect : nullptr,
			destinationRect ? &sdlDestinationRect : nullptr
		);
	}
}