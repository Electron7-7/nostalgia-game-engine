#ifndef TRANSFORM_3D_H
#define TRANSFORM_3D_H

#include "theatre_parser/fwd.hpp"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

class Transform3D
{
public:
    virtual void SetTransformVariables(const ThingData&);
    virtual void GetTransformVariables(ThingData&) const;

    virtual const glm::vec3& Origin() const;
    virtual const glm::vec3& Scale() const;
    virtual const glm::quat& Quaternion() const;
    virtual glm::vec3 Euler(bool Degrees = false) const;

    virtual void SetOrigin(const glm::vec3& NewOrigin);
    virtual void SetScale(const glm::vec3& NewScale);
    virtual void SetQuaternion(const glm::quat& NewQuaternion);
    virtual void SetEuler(const glm::vec3& NewEuler, bool Degrees = false);

    virtual glm::vec3 Front() const;
    virtual glm::vec3 Right() const;
    virtual glm::vec3 Up() const;

protected:
    glm::vec3 mOrigin     = glm::vec3(0.0f);
    glm::quat mQuaternion = glm::quat();
    glm::vec3 mEuler      = glm::vec3(0.0f);
    glm::vec3 mScale      = glm::vec3(1.0f);
};

#endif // TRANSFORM_3D_H
