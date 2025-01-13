#pragma once
#include <memory>
#include <unordered_map>

#include "Neela/Interfaces/IRenderable.h"

namespace Neela
{
	class Sprite;
}

namespace ACGame
{
	enum class CellType : std::uint8_t;
	struct GridData;

	class RenderableGrid : public Neela::IRenderable
	{
	private:
		mutable std::unordered_map<CellType, std::shared_ptr<Neela::Sprite>> m_sprites;
		const GridData* m_gridData;
		std::uint8_t m_renderLayer;

	public:
		RenderableGrid() = default;
		RenderableGrid(const GridData* gridData, std::uint8_t renderLayer = 0);

		void Draw(const Neela::Vector2i& position) const override;
		void Draw(const Neela::Transform& transform) const override;
		Neela::Rect GetBounds() const override;
		void SetTexture(const std::shared_ptr<Neela::Texture>& tex) override;
		const std::shared_ptr<Neela::Texture>& GetTexture() const override;
		const std::string& GetTexturePath() const override;
		std::uint8_t GetRenderLayer() const override;
		void SetRenderLayer(std::uint8_t newRenderLayer) override;
	private:
		std::shared_ptr<Neela::Sprite> GetCellSprite(CellType cellType) const;

	};
}
