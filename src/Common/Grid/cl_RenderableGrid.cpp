#include <Common/Grid/cl_RenderableGrid.h>

#include "Common/sh_Constants.h"
#include "Common/Grid/sh_GridData.h"
#include "Neela/Essentials/ResourcesManager.h"
#include "Neela/Essentials/WindowsManager.h"
#include "Neela/Graphics/Sprite.h"
#include "Neela/Graphics/Transform.h"

namespace ACGame
{
	RenderableGrid::RenderableGrid(const GridData* gridData, std::uint8_t renderLayer) :
		m_gridData(gridData),
		m_renderLayer(renderLayer)
	{
	}

	void RenderableGrid::Draw(const Neela::Vector2i& position) const
	{
		if (m_gridData->GetWidth() == 0 || m_gridData->GetHeight() == 0)
			return;

		Neela::Transform transform;
		transform.SetAbsolutePosition(position);
		// stretch sprite to fit cell size
		transform.SetAbsoluteScale(Neela::Vector2i::One() * (CellSize / static_cast<float>(GetCellSprite(DefaultCellType)->GetTexture()->GetSize().X)));

		for (std::uint8_t i = 0; i < m_gridData->GetWidth(); ++i)
		{
			for (std::uint8_t j = 0; j < m_gridData->GetHeight(); ++j)
			{
				CellType cell = m_gridData->GetCellType(i, j);
				if (cell == CellType::None)
					continue;

				std::shared_ptr<Neela::Sprite> sprite = GetCellSprite(cell);

				transform.SetAbsolutePosition(position + Neela::Vector2i(i * CellSize, j * CellSize));
				sprite->Draw(transform);
			}
		}
	}

	void RenderableGrid::Draw(const Neela::Transform& transform) const
	{
		this->Draw(transform.GetAbsolutePosition()); // No rotation/scale support for grids;
	}

	Neela::Rect RenderableGrid::GetBounds() const
	{
		// return empty grid, we don't care
		return Neela::Rect(0, 0);
	}

	void RenderableGrid::SetTexture(const std::shared_ptr<Neela::Texture>& tex)
	{
		throw;
	}

	const std::shared_ptr<Neela::Texture>& RenderableGrid::GetTexture() const
	{
		// return default cell texture, we don't care
		return GetCellSprite(DefaultCellType)->GetTexture();
	}

	const std::string& RenderableGrid::GetTexturePath() const
	{
		return GetTexture()->GetPath();
	}

	std::uint8_t RenderableGrid::GetRenderLayer() const
	{
		return m_renderLayer;
	}

	void RenderableGrid::SetRenderLayer(std::uint8_t newRenderLayer)
	{
		m_renderLayer = newRenderLayer;
		for (auto&& pair : m_sprites)
			pair.second->SetRenderLayer(m_renderLayer);
	}

	std::shared_ptr<Neela::Sprite> RenderableGrid::GetCellSprite(CellType cellType) const
	{
		if (m_sprites.contains(cellType))
			return m_sprites[cellType];

		std::string assetPath;
		switch (cellType)
		{
			case CellType::Wall:
				assetPath = TILE_ASSET_WALL;
				break;
			case CellType::InvisibleWall:
				assetPath = TILE_ASSET_INVISIBLE_WALL;
				break;
			case CellType::Grass:
				assetPath = TILE_ASSET_GRASS;
				break;
			case CellType::Grass2:
				assetPath = TILE_ASSET_GRASS_2;
				break;
			case CellType::Grass3:
				assetPath = TILE_ASSET_GRASS_3;
				break;
			case CellType::Dirt:
				assetPath = TILE_ASSET_DIRT;
				break;
			case CellType::Stone:
				assetPath = TILE_ASSET_STONE;
				break;
			default: throw;
		}

		std::shared_ptr<Neela::Texture> texture = Neela::ResourcesManager::GetTexture(
			Neela::WindowsManager::GetMainWindow(), assetPath);
		m_sprites[cellType] = std::make_shared<Neela::Sprite>(texture, nullptr, m_renderLayer);
		return m_sprites[cellType];
	}
}
