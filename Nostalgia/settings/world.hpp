#ifndef WORLD_SETTINGS_H
#define WORLD_SETTINGS_H

#ifdef NOSTALGIA_GCC_PCH
#   include <Nostalgia/Nostalgia.hpp>
#endif

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
