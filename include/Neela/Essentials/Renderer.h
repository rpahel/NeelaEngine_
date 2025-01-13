#pragma once

#include "Neela/Essentials/Export.h"
#include "Neela/Maths/Rect.h"

#include <SDL2/SDL.h>
#include <memory>

namespace Neela
{
	class Texture;
	class Window;
	struct Color;

	class NEELA_ENGINE_API Renderer
	{
		friend class Texture;

	private:
		std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> m_SdlRenderer;

	public:
		//==== Constructors ====

		Renderer(Window* window, int index = -1, uint32_t flags = 0);
		Renderer(const Renderer&) = delete;
		Renderer(Renderer&& victim) noexcept;
		~Renderer();

		//==== Public Methods ====

		void Clear() const;
		void Present() const;
		void SetBackgroundColor(const Color& newColor) const;

		//==== Operators ====

		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&& victim) noexcept;
		operator SDL_Renderer* ();

	private:
		//==== Methods ====

		void DrawTexture(const Texture* texture, Rect* destinationRect = nullptr, Rect* sourceRect = nullptr);
	};
}