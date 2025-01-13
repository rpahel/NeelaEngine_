#pragma once
#include "Common/sh_Constants.h"
#include "Neela/Maths/Vector.h"

namespace ACGame
{
    struct SpawnerComponent
    {
        Neela::Vector2f topLeftSpawnZone;
        std::uint8_t columnCount = 3;
        std::uint8_t rowCount = 3;

        std::uint8_t spawnedCount = 0;

        std::uint32_t spawnPrice = 0;

        bool CanSpawn() const
        {
            return spawnedCount < columnCount * rowCount;
        }

        Neela::Vector2f GetNextSpawnPos() const
        {
            return topLeftSpawnZone + Neela::Vector2i(CellSize * (spawnedCount % columnCount), CellSize * (spawnedCount / rowCount));
        }
    };
}
