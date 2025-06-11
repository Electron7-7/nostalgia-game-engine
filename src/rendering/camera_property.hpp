#ifndef CAMERAPROPERTY_H
#define CAMERAPROPERTY_H

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

class CameraProperty
{
public:
    void UpdateView();

    glm::vec3 GetFront();
    glm::vec3 GetRight();
    glm::vec3 GetUp();

    glm::vec3 origin = glm::vec3(0.0f);
    glm::quat rotation = glm::quat();
};
#endif // CAMERAPROPERTY_H