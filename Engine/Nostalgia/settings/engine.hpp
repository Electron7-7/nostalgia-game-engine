#ifndef ENGINE_SETTINGS_H
#define ENGINE_SETTINGS_H

namespace Settings
{
    namespace Engine
    {
        inline int  TickRate{70};
        inline bool IsEditorHint{false};

        inline float TickInterval()
        { return (1.0f / TickRate); }
    }
}

#endif // ENGINE_SETTINGS_H
