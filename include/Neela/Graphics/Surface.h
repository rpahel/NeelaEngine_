#pragma once

#include "Neela/Essentials/Export.h"
#include "Neela/Maths/Vector.h"

#include <SDL2/SDL.h>

namespace Neela
{
	class NEELA_ENGINE_API Surface
	{
	private:
		SDL_Surface* m_SdlSurface;
		std::string m_Path;

	public:
		//==== Constructors ====

		Surface(const Surface& copy) = delete;
		Surface(Surface&& victim) noexcept;
		~Surface();

		//===== Methods ====

		Vector2i GetSize() const;
		SDL_Surface* GetSdlSurface();
		std::string GetPath() const;

		//===== Static Methods ====

		static Surface LoadFromFile(const char* filePath);
		static Surface GetDefault();

		//==== Operators ====

		Surface& operator=(const Surface& copy) = delete;
		Surface& operator=(Surface&& victim) noexcept;
		operator SDL_Surface* ();

	private:
		//==== Constructors ====

		explicit Surface();
		explicit Surface(const char* filePath);

		//==== Methods ====

		void GenerateDefaultSurface();
	};
}