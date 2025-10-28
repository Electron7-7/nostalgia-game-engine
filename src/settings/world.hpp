#ifndef WORLD_SETTINGS_H
#define WORLD_SETTINGS_H

#include "common/fwd.hpp"

#include <glm/fwd.hpp>

namespace Settings
{
    namespace World
    {
        extern const glm::vec3& Up();
        extern const glm::vec3& Right();
        extern const glm::vec3& Front();

        extern bool UpdateOrientation(const ID& GraphicsLibraryID);
    }
}

#endif // WORLD_SETTINGS_H
