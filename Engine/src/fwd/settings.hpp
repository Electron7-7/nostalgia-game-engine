#ifndef SETTINGS_FWD_H
#define SETTINGS_FWD_H

#include <glm/fwd.hpp>

namespace Settings
{
    namespace Engine
    {
        extern int TickRate;
        float TickInterval();
    }

    namespace Graphics
    {
        extern bool GlobalWireframe;
    }

    namespace Player
    {
        extern float FOV,
            ViewCutoffNear,
            ViewCutoffFar,
            MouseSensitivity,
            MouseSensitivityScale,
            MovementSpeed;
        extern bool RawMouseMotion;
    }

    namespace World
    {
        const glm::vec3& Up();
        const glm::vec3& Right();
        const glm::vec3& Front();
    }
}

#endif // SETTINGS_FWD_H
