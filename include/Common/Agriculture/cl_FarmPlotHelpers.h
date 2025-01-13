#pragma once
#include <entt/entity/handle.hpp>
#include <vector>
#include <string>

#include "sh_FarmPlotComponent.h"
#include "Neela/Graphics/Texture.h"

namespace ACGame
{
    struct FarmPlantTypeTextures
    {
        FarmPlantTypeTextures(FarmPlantType farm_plant_type, const std::string& growing_texture,
            const std::string& grown_texure)
            : farmPlantType(farm_plant_type),
              growingTexture(growing_texture),
              grownTexure(grown_texure)
        {
        }

        FarmPlantType farmPlantType;
        std::string growingTexture;
        std::string grownTexure;
    };
    
    class FarmPlotHelpers
    {
    public:
        static void UpdateFarmPlotVisual(entt::handle handle, const FarmPlotComponent& farmPlot);

        static std::shared_ptr<Neela::Texture> GetCorrespondingTexture(FarmPlotState farmPlotState, FarmPlantType farmPlantType);
        static const FarmPlantTypeTextures& GetFarmPlantTypeTextures(FarmPlantType farmPlantType);
        
    private:
        static std::vector<FarmPlantTypeTextures> s_farmPlotTexturesPath;
    };
    
}
