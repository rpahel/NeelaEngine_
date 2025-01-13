#pragma once

#include "Neela/Essentials/Export.h"
#include "Neela/Maths/Vector.h"

struct SDL_Texture;

namespace Neela
{
	struct Rect;
	class Window;
	class Renderer;
	class Surface;
	class Transform;

	class NEELA_ENGINE_API Texture
	{
		friend class ResourcesManager;

	private:
		SDL_Texture* m_SdlTexture;
		std::weak_ptr<Window> m_Window;
		std::string m_Path;

	public:
		//==== Constructors ====

		Texture(const Texture& copy) = delete;
		Texture(Texture&& victim) noexcept;
		~Texture();

		//==== Operators ====

		Texture& operator=(const Texture& copy) = delete;
		Texture& operator=(Texture&& victim) noexcept;

		//==== Methods ====

		Vector2i GetSize() const;
		SDL_Texture* GetSdlTexture() const;
		Renderer* GetRenderer() const;
		const std::weak_ptr<Window>& GetWindow() const;
		std::string GetPath() const;

	private:
		//==== Constructors ====

		explicit Texture(const std::weak_ptr<Window>& window, const char* imagePath);
		explicit Texture(const std::weak_ptr<Window>& window, const Surface& surface);

		//==== Static Methods ====

		static Texture LoadFromFile(const std::weak_ptr<Window>& window, const char* imagePath);
	};
}