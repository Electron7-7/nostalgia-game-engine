#ifndef TRANSFORM_3D_H
#define TRANSFORM_3D_H

#define FWD_DCL
#   include "theatre/parser/thing_data.hpp"
#undef  FWD_DCL

#include "core/farg.hpp"
#include "core/smart_pointers.hpp"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

class Transform3D
{
public:
    virtual void SetTransformVariables(Farg<ThingData>);
    virtual void GetTransformVariables(Shared<ThingData>) const;

    virtual Farg<glm::vec3> Origin() const;
    virtual Farg<glm::vec3> Scale() const;
    virtual Farg<glm::quat> Quaternion() const;
    virtual glm::vec3 Euler(bool Degrees = false) const;

    virtual void SetOrigin(Farg<glm::vec3> NewOrigin);
    virtual void SetScale(Farg<glm::vec3> NewScale);
    virtual void SetQuaternion(Farg<glm::quat> NewQuaternion);
    virtual void SetEuler(Farg<glm::vec3> NewEuler, bool Degrees = false);

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
