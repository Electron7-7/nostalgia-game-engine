#ifndef ENGINE_SETTINGS_H
#define ENGINE_SETTINGS_H

#include "common/fwd.hpp"

namespace Settings
{
    namespace Engine
    {
        extern int   TickRate;
        extern float TickInterval();
        extern ID    GraphicsBackend;
        extern ID    WindowingBackend;
    }
}

#endif // ENGINE_SETTINGS_H
