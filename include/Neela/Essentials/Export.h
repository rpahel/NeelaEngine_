#pragma once

#ifdef NEELA_ENGINE_STATIC
    #define NEELA_ENGINE_API
#else
    #ifdef NEELA_ENGINE_BUILD
        #define NEELA_ENGINE_API __declspec(dllexport)
    #else
        #define NEELA_ENGINE_API __declspec(dllimport)
    #endif
#endif