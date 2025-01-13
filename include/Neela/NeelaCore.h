#pragma once

//==== Essentials
#include "Neela/Essentials/Components.h"
#include "Neela/Essentials/Core.h"
#include "Neela/Essentials/EntityCreator.h"
#include "Neela/Essentials/Export.h"
#include "Neela/Essentials/InputsManager.h"
#include "Neela/Essentials/Game.h"
#include "Neela/Essentials/Renderer.h"
#include "Neela/Essentials/ResourcesManager.h"
#include "Neela/Essentials/Serializer.h"
#include "Neela/Essentials/Systems.h"
#include "Neela/Essentials/Timer.h"
#include "Neela/Essentials/Window.h"
#include "Neela/Essentials/WindowsManager.h"

//==== Enums
#include "Neela/Enums/GetWindowBehaviourEnum.h"
#include "Neela/Enums/MouseButtonEnum.h"

//==== Graphics
#include "Neela/Graphics/AnimationData.h"
#include "Neela/Graphics/Color.h"
#include "Neela/Graphics/Model.h"
#include "Neela/Graphics/Sprite.h"
#include "Neela/Graphics/SpriteSheet.h"
#include "Neela/Graphics/Surface.h"
#include "Neela/Graphics/Texture.h"
#include "Neela/Graphics/Transform.h"

//==== Audio
#include "Neela/Audio/AudioWave.h"

//==== Inputs
#include "Neela/Inputs/InputAction.h"

//==== Interfaces
#include "Neela/Interfaces/IPrintable.h"
#include "Neela/Interfaces/IRenderable.h"
#include "Neela/Interfaces/IFileSerializeable.h"
#include "Neela/Interfaces/ISerializeable.h"

//==== Maths
#include "Neela/Maths/Maths.h"
#include "Neela/Maths/Matrix.h"
#include "Neela/Maths/Rect.h"
#include "Neela/Maths/Vector.h"

//==== Other
#include "Neela/ImGuiRenderer.h"
#include "Neela/NeelaConstants.h"

//==== Externals
#include <entt/entt.hpp>
#include <fmt/core.h>
#include <fmt/color.h>
#include <nlohmann/json.hpp>