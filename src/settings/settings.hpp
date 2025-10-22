#ifndef SETTINGS_H
#define SETTINGS_H

#include "common/safe_return.hpp"

#include <glm/fwd.hpp>

namespace Settings
{
    // The 'SetSetting' and 'try_Get-' functions are for users to define their own settings without needing to recompile
    // the library. However, there will also be structs full of static settings that can't be removed; these are settings
    // that are more universal, like window width/height, certain graphics options, and the tick rate. Use the functions
    // (and, consequently, the maps) when adding custom options to the engine/game. Use the static variables when their
    // specific purpose is required (e.g: setting the window's fullscreen state).

    extern void SetSetting(const char* SettingName, const char* Setting);
    extern void SetSetting(const char* SettingName, float Setting);
    extern void SetSetting(const char* SettingName, int Setting);

    extern SafeReturn<const char*> try_GetString(const char* SettingName);
    extern SafeReturn<float>       try_GetFloat(const char* SettingName);
    extern SafeReturn<int>         try_GetInt(const char* SettingName);

    namespace Engine
    {
        extern int   TickRate;
        extern float TickInterval();
        extern int   GraphicsBackend;
        extern int   WindowingBackend;
    }

    namespace World
    {
        extern const glm::vec3& Up();
        extern const glm::vec3& Right();
        extern const glm::vec3& Front();

        extern bool UpdateOrientation(const ID& GraphicsLibraryID);
    }

    namespace Window
    {
        extern int Width;
        extern int Height;
        extern int XPosition;
        extern int YPosition;
        extern int FullscreenWidth;
        extern int FullscreenHeight;
        extern int FullscreenXPosition;
        extern int FullscreenYPosition;
        extern bool Fullscreen;
        extern const char* Name;
    }

    namespace Player
    {
        extern float FOV;
        extern float ViewCutoffNear;
        extern float ViewCutoffFar;
        extern bool  RawMouseMotion;
        extern float MouseSensitivity;
        extern float MouseSensitivityScale;
        extern float MovementSpeed;
    }

    namespace Graphics
    {
        extern bool GlobalWireframe;
    }
}

#endif // SETTINGS_H
