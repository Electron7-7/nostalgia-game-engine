#ifndef CAMERA_3D_H
#define CAMERA_3D_H

#include "actor.hpp"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

class Camera3D : public Actor
{
public:
    glm::vec3 GetFront();
    glm::vec3 GetRight();
    glm::vec3 GetUp();
};
#endif // CAMERA_3D_H
