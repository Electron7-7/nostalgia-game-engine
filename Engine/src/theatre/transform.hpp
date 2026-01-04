#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "fwd/theatre.hpp"
#include "core/farg.hpp"
#include "core/mutex.hpp"
#include "core/smart_pointers.hpp"
#include <glm/vec3.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

class Transform2D
{
public:
    void SetTransformVariables(Farg<ThingData>);
    void GetTransformVariables(Shared<ThingData>) const;

    virtual Farg<glm::vec2> Origin() const;
    virtual Farg<glm::vec2> Scale() const;
    virtual double Rotation(bool isDegrees = false) const;

    virtual void Origin(Farg<glm::vec2> inOrigin);
    virtual void Scale(Farg<glm::vec2> inScale);
    virtual void Rotation(double inRotation, bool isDegrees = false);

private:
    glm::vec2 mOrigin{},
        mScale{};
    double mRotation{};
    RMutex mTransformMutex{};
};

class Transform3D
{
public:
    void SetTransformVariables(Farg<ThingData>);
    void GetTransformVariables(Shared<ThingData>) const;

    Farg<glm::vec3> Origin() const;
    Farg<glm::vec3> Scale() const;
    Farg<glm::quat> Quaternion() const;
    glm::vec3 Euler(bool isDegrees = false) const;

    glm::vec3 Front() const;
    glm::vec3 Right() const;
    glm::vec3 Up() const;

    virtual void Origin(Farg<glm::vec3> inOrigin);
    virtual void Scale(Farg<glm::vec3> inScale);
    virtual void Quaternion(Farg<glm::quat> inQuaternion);
    virtual void Euler(Farg<glm::vec3> inEuler, bool isDegrees = false);

protected:
    glm::vec3 mOrigin{0.0f},
        mEuler{0.0f},
        mScale{1.0f};
    glm::quat mQuaternion{};
    RMutex mTransformMutex{};
};

#endif // TRANSFORM_H
