#pragma once
#include "sh_FarmPlantType.h"

namespace ACGame
{
    enum class FarmPlotState
    {
        None = 0,
        Empty = 1,
        Growing = 2,
        Grown = 3
    };
    
    struct FarmPlotComponent
    {
        FarmPlotState state = FarmPlotState::Empty;
        FarmPlantType plantType = FarmPlantType::None;
        float growTimer = 0.0;
    };
}
