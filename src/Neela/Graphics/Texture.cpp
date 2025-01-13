#include "Neela/Essentials/Window.h"
#include "Neela/Essentials/Renderer.h"
#include "Neela/Graphics/Texture.h"
#include "Neela/Graphics/Surface.h"
#include "Neela/Graphics/Transform.h"
#include "Neela/Maths/Rect.h"

#include <SDL_image.h>
#include <stdexcept>

namespace Neela
{
	//=====================================================================================================
	//==== PUBLIC CONSTRUCTORS
	//=====================================================================================================

	Texture::Texture(Texture&& victim) noexcept
	{
		m_SdlTexture = victim.m_SdlTexture;
		victim.m_SdlTexture = nullptr;
		m_Window = victim.m_Window;
	}

	Texture::~Texture()
	{
		SDL_DestroyTexture(m_SdlTexture);
	}

	//=====================================================================================================
	//==== PUBLIC OPERATORS
	//=====================================================================================================

	Texture& Texture::operator=(Texture&& victim) noexcept
	{
		std::swap(m_SdlTexture, victim.m_SdlTexture);
		return *this;
	}

	//=====================================================================================================
	//==== PUBLIC METHODS
	//=====================================================================================================

	Vector2i Texture::GetSize() const
	{
		Vector2i size;
		SDL_QueryTexture(m_SdlTexture, nullptr, nullptr, &size.X, &size.Y);
		return size;
	}

	SDL_Texture* Texture::GetSdlTexture() const
	{
		return m_SdlTexture;
	}

	Renderer* Texture::GetRenderer() const
	{
		return m_Window.lock() ? m_Window.lock()->GetRenderer().lock().get() : nullptr;
	}

	const std::weak_ptr<Window>& Texture::GetWindow() const
	{
		return m_Window;
	}

	std::string Texture::GetPath() const
	{
		return m_Path;
	}

	//=====================================================================================================
	//==== PRIVATE CONSTRUCTORS
	//=====================================================================================================

	Texture::Texture(const std::weak_ptr<Window>& window, const char* imagePath) :
		Texture::Texture(window, Surface::LoadFromFile(imagePath))
	{
	}

	Texture::Texture(const std::weak_ptr<Window>& window, const Surface& surface)
	{
		m_Window = window;
		m_Path = surface.GetPath();

		assert(m_Window.lock());
		try
		{
			if ((m_SdlTexture = SDL_CreateTextureFromSurface(*(m_Window.lock()->GetRenderer().lock()), const_cast<Surface&>(surface))) == nullptr)
				throw std::runtime_error("Texture::Texture(const Renderer& renderer, const Surface& surface) : Failed to create m_SdlTexture.");
		}
		catch (const std::exception& e)
		{
			printf("%s", e.what());
			printf("%s", SDL_GetError());
			exit(EXIT_FAILURE);
		}
	}

	//=====================================================================================================
	//==== PRIVATE STATIC METHODS
	//=====================================================================================================

	Texture Texture::LoadFromFile(const std::weak_ptr<Window>& window, const char* imagePath)
	{
		return Texture(window, imagePath);
	}
}