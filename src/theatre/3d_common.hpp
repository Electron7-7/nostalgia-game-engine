#ifndef THREE_DEE_COMMON_H
#define THREE_DEE_COMMON_H

#include <glm/fwd.hpp>

namespace World
{
    namespace Orientation
    {
        glm::vec3 Up();
        glm::vec3 Right();
        glm::vec3 Front();

        // TODO: Being able to change the global orientation vectors might not be a good idea, because it could fuck up code that relies on them being a certain value...
        void SetWorldUp(glm::vec3 Up);
        void SetWorldRight(glm::vec3 Right);
        void SetWorldFront(glm::vec3 Front);
    }
}

#endif // THREE_DEE_COMMON_H
