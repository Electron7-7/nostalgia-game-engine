#ifndef POSITION_3D_H
#define POSITION_3D_H

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

class Position3D
{
public:
    virtual void SetOrigin(const glm::vec3& Origin);
    virtual void SetRotation(const glm::vec3& Rotation, bool Degrees = false);
    virtual void SetQuaternion(const glm::quat& Quaternion);
    virtual void SetScale(const glm::vec3& Scale);

    virtual glm::vec3 Origin() const;
    virtual glm::vec3 Rotation(bool Degrees = false) const;
    virtual glm::quat Quaternion() const;
    virtual glm::vec3 Scale() const;

    virtual glm::vec3 Front() const;
    virtual glm::vec3 Right() const;
    virtual glm::vec3 Up() const;

protected:
    glm::vec3 m_Origin = glm::vec3(0.0f);
    glm::vec3 m_Rotation = glm::vec3(0.0f);
    glm::vec3 m_Scale = glm::vec3(1.0f);
};

#endif // POSITION_3D_H
