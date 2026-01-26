#ifndef WORLD_SETTINGS_H
#define WORLD_SETTINGS_H

#include <glm/fwd.hpp>

namespace Settings
{
    namespace World
    {
        extern Farg<glm::vec3> Up();
        extern Farg<glm::vec3> Right();
        extern Farg<glm::vec3> Front();
    }
}

#endif // WORLD_SETTINGS_H
