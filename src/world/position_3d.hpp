#ifndef POSITION_3D_H
#define POSITION_3D_H

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

class Position3D
{
public:
    virtual void SetOrigin(const glm::vec3& Origin);
    virtual void SetRotation(const glm::quat& Rotation);
    virtual const glm::vec3& Origin() const;
    virtual const glm::quat& Rotation() const;

    virtual glm::vec3 Front() const;
    virtual glm::vec3 Right() const;
    virtual glm::vec3 Up() const;

protected:
    glm::vec3 m_Origin   = glm::vec3(0.0f);
    glm::quat m_Rotation = glm::quat();
};

#endif // POSITION_3D_H
