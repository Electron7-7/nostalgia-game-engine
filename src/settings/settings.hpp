#ifndef SETTINGS_H
#define SETTINGS_H

#include "safe_return.hpp"

struct Settings
{
public:
    // The 'SetSetting' and 'try_Get-' functions are for users to define their own settings without needing to recompile
    // the library. However, there will also be structs full of static settings that can't be removed; these are settings
    // that are more universal, like window width/height, certain graphics options, and the tick rate. Use the functions
    // (and, consequently, the maps) when adding custom options to the engine/game. Use the static variables when their
    // specific purpose is required (e.g: setting the window's fullscreen state).

    static void SetSetting(const char* SettingName, const char* Setting);
    static void SetSetting(const char* SettingName, float Setting);
    static void SetSetting(const char* SettingName, int Setting);

    static SafeReturn<const char*> try_GetString(const char* SettingName);
    static SafeReturn<float>       try_GetFloat(const char* SettingName);
    static SafeReturn<int>         try_GetInt(const char* SettingName);

    struct Engine
    {
        static int   TickRate;
        static float TickInterval;
    };

    struct Window
    {
        static int Width;
        static int Height;
        static int FullscreenWidth;
        static int FullscreenHeight;
        static int XPosition;
        static int YPosition;
        static bool Fullscreen;
        static const char* Name;
    };

    struct Player
    {
        static float FOV; // TODO: Replace with horizontal & vertical FOVs?
        static float ViewCutoffNear;
        static float ViewCutoffFar;
    };

    struct Graphics
    {};
};

#endif // SETTINGS_H
