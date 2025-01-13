#pragma once
#include <cstdint>

namespace ACGame
{
    enum class GenericInteractableEntitiesType : std::uint8_t
    {
        None = 0, // SHOULD NOT BE USED
        Buy = 1,
        Upgrade = 2,
        Automate = 3,
    };

    struct GenericInteractableComponent
    {
        GenericInteractableEntitiesType type;
    };
}
