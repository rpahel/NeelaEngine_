#include "Common/Agriculture/cl_FarmPlotHelpers.h"

#include "Neela/Components/GraphicsComponent.h"
#include "Common/cl_TextureConstants.h"
#include "Neela/Essentials/ResourcesManager.h"
#include "Neela/Essentials/Window.h"
#include "Neela/Essentials/WindowsManager.h"

namespace ACGame
{
    std::vector<FarmPlantTypeTextures> FarmPlotHelpers::s_farmPlotTexturesPath =
    {
        FarmPlantTypeTextures(FarmPlantType::Carrot, ASSET_CARROT_GROWING, ASSET_CARROT_GROWN),
        FarmPlantTypeTextures(FarmPlantType::Rabbit, ASSET_RABBIT_GROWING, ASSET_RABBIT_GROWN),
        FarmPlantTypeTextures(FarmPlantType::Third, ASSET_PLANT_THIRD_GROWING, ASSET_PLANT_THIRD_GROWN),
        FarmPlantTypeTextures(FarmPlantType::Fourth, ASSET_PLANT_FOURTH_GROWING, ASSET_PLANT_FOURTH_GROWN),
        FarmPlantTypeTextures(FarmPlantType::Fifth, ASSET_PLANT_FIFTH_GROWING, ASSET_PLANT_FIFTH_GROWN),
        FarmPlantTypeTextures(FarmPlantType::Sixth, ASSET_PLANT_SIXTH_GROWING, ASSET_PLANT_SIXTH_GROWN),
    };

    void FarmPlotHelpers::UpdateFarmPlotVisual(entt::handle handle, const FarmPlotComponent& farmPlot)
    {
        if (Neela::GraphicsComponent* graphicsComponent = handle.try_get<Neela::GraphicsComponent>())
        {
            graphicsComponent->m_Renderable->SetTexture(GetCorrespondingTexture(farmPlot.state, farmPlot.plantType));
        }
    }

    std::shared_ptr<Neela::Texture> FarmPlotHelpers::GetCorrespondingTexture(FarmPlotState farmPlotState, FarmPlantType farmPlantType)
    {
        if(farmPlotState == FarmPlotState::Empty || farmPlotState == FarmPlotState::None)
            return nullptr;

        if(farmPlantType == FarmPlantType::None)
            return nullptr;

        std::string texturePath = "";
        FarmPlantTypeTextures textures = GetFarmPlantTypeTextures(farmPlantType);
        switch (farmPlotState)
        {
        case FarmPlotState::None:
        case FarmPlotState::Empty:
            return nullptr;
        case FarmPlotState::Growing:
            texturePath = textures.growingTexture;
            break;
        case FarmPlotState::Grown:
            texturePath = textures.grownTexure;
            break;
        default: throw;
        }

        return Neela::ResourcesManager::GetTexture(Neela::WindowsManager::GetMainWindow(), texturePath);
    }

    const FarmPlantTypeTextures& FarmPlotHelpers::GetFarmPlantTypeTextures(FarmPlantType farmPlantType)
    {
        for (int i = 0; i < s_farmPlotTexturesPath.size(); ++i)
        {
            if(s_farmPlotTexturesPath[i].farmPlantType == farmPlantType)
                return s_farmPlotTexturesPath[i];
        }

        throw std::runtime_error(std::string("[CLIENT] No textures found for the farm plant type: ") + FarmPlantTypeNames[static_cast<std::uint8_t>(farmPlantType)]);
    }
}
