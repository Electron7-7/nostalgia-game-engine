#ifndef WORLD_H
#define WORLD_H

#include <glm/fwd.hpp>

struct World
{
    static const glm::vec3& Up();
    static const glm::vec3& Right();
    static const glm::vec3& Front();

    static void SetUp(const glm::vec3& NewUp);
    static void SetRight(const glm::vec3& NewRight);
    static void SetFront(const glm::vec3& NewFront);
};

#endif // WORLD_H
