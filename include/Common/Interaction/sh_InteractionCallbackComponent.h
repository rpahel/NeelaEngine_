#pragma once
#include <functional>

namespace ACGame
{
    using InteractionCallback = std::function<void(entt::handle handle, std::uint8_t actorClientId)>;
    
    struct InteractionCallbackComponent
    {
        InteractionCallback interactionCallback;

        void Invoke(entt::handle handle, std::uint8_t actorClientId) const
        {
            if(interactionCallback)
                interactionCallback(handle, actorClientId);
        }
    };
}
