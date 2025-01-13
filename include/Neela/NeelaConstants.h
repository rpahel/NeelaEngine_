#pragma once

#define DEFAULT_FRAME_RATE 60.0f
#define DEFAULT_FRAME_DURATION (1000.0f/DEFAULT_FRAME_RATE)

#define ASSETS_ROOT std::string("assets/")
#define TEXTURES_ROOT (ASSETS_ROOT + "Textures/")
#define SPRITESHEETS_ROOT (TEXTURES_ROOT + "SpriteSheets/")
#define CHARACTER_TEXTURES_ROOT (SPRITESHEETS_ROOT + "Characters/")
#define AUDIO_ROOT (ASSETS_ROOT + "Audio/")
#define SPRITES_ROOT (ASSETS_ROOT + "Sprites/")
#define MODELS_ROOT (ASSETS_ROOT + "Models/")
#define SCENES_ROOT (ASSETS_ROOT + "Scenes/")

#define BINARY_EXTENSION ".nab"
#define JSON_EXTENSION ".naj"

#define SAVEFILES_ROOT std::string("saves/")
#define SAVEFILES_AUDIOWAVES (SAVEFILES_ROOT + "AudioWaves/")
#define SAVEFILES_INPUTS (SAVEFILES_ROOT + "Inputs/")
#define SAVEFILES_SPRITESHEETS (SAVEFILES_ROOT + "SpriteSheets/")
#define SAVEFILES_SPRITES (SAVEFILES_ROOT + "Sprites/")
#define SAVEFILES_MODELS (SAVEFILES_ROOT + "Models/")

#define G_VALUE 9.81f
#define G_SCALE_DEFAULT 1000.0f

#define AUDIO_DEVICE nullptr