#include "Neela/Graphics/Surface.h"
//#include "Neela/Debug.h"

#include <SDL_image.h>
#include <stdexcept>

namespace Neela
{
	//=====================================================================================================
	//==== PUBLIC CONSTRUCTORS
	//=====================================================================================================

	Surface::Surface(Surface&& victim) noexcept
	{
		m_SdlSurface = victim.m_SdlSurface;
		victim.m_SdlSurface = nullptr;
	}

	Surface::~Surface()
	{
		SDL_FreeSurface(m_SdlSurface);
	}

	//=====================================================================================================
	//==== PUBLIC METHODS
	//=====================================================================================================

	Vector2i Surface::GetSize() const
	{
		if (!m_SdlSurface)
			return Vector2i(0);

		return Vector2i(m_SdlSurface->w, m_SdlSurface->h);
	}

	SDL_Surface* Surface::GetSdlSurface()
	{
		return m_SdlSurface;
	}

	std::string Surface::GetPath() const
	{
		return m_Path;
	}

	//=====================================================================================================
	//==== PUBLIC STATIC METHODS
	//=====================================================================================================

	Surface Surface::LoadFromFile(const char* filePath)
	{
		return Surface(filePath);
	}

	Surface Surface::GetDefault()
	{
		return Surface();
	}

	//=====================================================================================================
	//==== PUBLIC OPERATORS
	//=====================================================================================================

	Surface& Surface::operator=(Surface&& victim) noexcept
	{
		std::swap(m_SdlSurface, victim.m_SdlSurface);
		return *this;
	}

	Surface::operator SDL_Surface* ()
	{
		return m_SdlSurface;
	}

	//=====================================================================================================
	//==== PRIVATE CONSTRUCTORS
	//=====================================================================================================

	Surface::Surface()
	{
		m_Path = "";
		GenerateDefaultSurface();
	}

	Surface::Surface(const char* filePath)
	{
		m_Path = filePath;
		if ((m_SdlSurface = IMG_Load(filePath)) == nullptr)
		{
			fmt::print(fg(fmt::color::red), "Surface::Surface(const char* filePath) : No file found at {0}. \n", std::string(filePath));
			GenerateDefaultSurface();

			if (m_SdlSurface == nullptr)
				throw std::runtime_error("Surface::Surface(const char* filePath) : Failed to create surface.");
		}
	}

	//=====================================================================================================
	//==== PRIVATE METHODS
	//=====================================================================================================

	void Surface::GenerateDefaultSurface()
	{
		m_SdlSurface = SDL_CreateRGBSurface(0, 100, 100, 16, 0, 0, 0, 0);

		if (m_SdlSurface == nullptr)
			throw std::runtime_error("Surface::Surface() : Failed to create surface.");

		SDL_Rect rects[25]{};
		int k = 0;
		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				rects[k] = { j * 20, i * 20, 20, 20 };

				SDL_FillRect(
					m_SdlSurface,
					&rects[k],
					SDL_MapRGB(
						m_SdlSurface->format,
						k % 2 == 0 ? 255 : 0,
						0,
						k % 2 == 0 ? 255 : 0
					)
				);

				k++;
			}
		}
	}
}