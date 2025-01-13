#pragma once
#include <cstdint>
#include <unordered_map>

namespace ACGame
{
    enum class FarmPlantType : std::uint8_t
    {
        None = 0,
        Carrot,
        Rabbit,
        Third,
        Fourth,
        Fifth,
        Sixth
    };

    struct FarmPlantConfig
    {
        float growingDuration;
        std::uint32_t plantationPrice; // cost to plant the type of farm plant
        std::uint32_t harvestGain; // money gained by harvesting this type of farm plant
        std::uint32_t spawnPrice; // cost to spawn a farm plot of this type
        std::uint32_t upgradePrice; // cost to upgrade the harvest gain
        std::uint32_t automationPrice; // cost to automate the harvest and plantation

        bool automated = false; // is this type automated (will change at runtime)
    };

    inline std::unordered_map<FarmPlantType, FarmPlantConfig> FarmPlantConfigs(
        {
            {
                FarmPlantType::Carrot, FarmPlantConfig{2.0, 1, 2, 10, 1'000, 5'000}
            },
            {
                FarmPlantType::Rabbit, FarmPlantConfig{5.0, 10, 30, 100, 5'000, 20'000}
            },
            {
                FarmPlantType::Third, FarmPlantConfig{10.0, 20, 80, 200, 20'000, 50'000}
            },
            {
                FarmPlantType::Fourth, FarmPlantConfig{20.0, 100, 400, 1'000, 50'000, 200'000}
            },
            {
                FarmPlantType::Fifth, FarmPlantConfig{5.0, 1'000, 5'000, 10'000, 200'000, 1'000'000}
            },
            {
                FarmPlantType::Sixth, FarmPlantConfig{}
            },
        });
    
    inline std::vector<const char*> FarmPlantTypeNames({
        "None",
        "Carrot",
        "Rabbit",
        "ThirdFarmPlantType",
        "FourthFarmPlantType",
        "FifthFarmPlantType",
        "SixthFarmPlantType",
    });
}
