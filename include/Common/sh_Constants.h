#pragma once
#include <cstdint>

#define ASSETS_ROOT std::string("assets/")
#define TILES_ROOT (ASSETS_ROOT + "Kenney/Tiles/")
#define TILE_ASSET_GRASS (TILES_ROOT + "tile_0000.png")
#define TILE_ASSET_GRASS_2 (TILES_ROOT + "tile_0001.png")
#define TILE_ASSET_GRASS_3 (TILES_ROOT + "tile_0002.png")
#define TILE_ASSET_DIRT (TILES_ROOT + "tile_0025.png")
#define TILE_ASSET_STONE (TILES_ROOT + "tile_0043.png")
#define TILE_ASSET_WALL (TILES_ROOT + "tile_0126.png")
#define TILE_ASSET_INVISIBLE_WALL (TILE_ASSET_GRASS)

namespace ACGame
{
	constexpr std::uint32_t GameTickRate = 30;
	constexpr double GameTickDelay = 1. / GameTickRate;

	constexpr std::uint8_t CellSize = 32;

	constexpr int WindowWidth = 1280;
	constexpr int WindowHeight = 720;

#define TopLeftWindowPosX -640
#define TopLeftWindowPosY -360
#define TopLeftWindowPos Neela::Vector2f(TopLeftWindowPosX, TopLeftWindowPosY)

	constexpr std::uint32_t StartMoneyCount = 200;
	constexpr std::uint8_t PlayerInteractionRange = CellSize * 4;
	constexpr std::uint16_t PlayerInteractionRangeSquared = PlayerInteractionRange * PlayerInteractionRange;
}

namespace ACNet
{
	constexpr std::uint16_t AppPort = 14768;

	constexpr std::uint32_t SV_NetworkTickRate = 10;
	constexpr double SV_NetworkTickDelay = 1. / SV_NetworkTickRate;
	constexpr std::uint8_t SV_TargetJitterBufferSize = 5;
	constexpr float PredictionTolerance = (float)ACGame::CellSize / 4.0f;
	constexpr float PredictionToleranceSqrd = PredictionTolerance * PredictionTolerance;
	constexpr std::uint8_t TargetInterpBufferSize = 5;
}
