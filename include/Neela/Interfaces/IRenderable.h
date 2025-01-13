#pragma once

#include "Neela/Essentials/Export.h"
#include "Neela/Maths/Vector.h"

#include <memory>
#include <string>

namespace Neela
{
	class Transform;
	class Texture;
	struct Rect;

	class NEELA_ENGINE_API IRenderable
	{
	public:
		virtual void Draw(const Vector2i& position) const { return; }
		virtual void Draw(const Transform& transform) const { return; }
		virtual void Draw(Rect* destinationRect = nullptr, Rect* sourceRect = nullptr) const { return; }
		virtual Rect GetBounds() const = 0;

		virtual void SetTexture(const std::shared_ptr<Texture>& tex) = 0;
		virtual const std::shared_ptr<Texture>& GetTexture() const = 0;
		virtual const std::string& GetTexturePath() const = 0;
		virtual std::uint8_t GetRenderLayer() const = 0;
		virtual void SetRenderLayer(std::uint8_t newRenderLayer) = 0;
	};
}