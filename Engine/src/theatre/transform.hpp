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
protected:
    enum PropertyChanged : int
    { ORIGIN, ROTATION, SCALE };

    glm::vec2 mOrigin{},
        mScale{};
    double mRotation{};
    RMutex mTransformMutex{};

public:
    virtual void OnTransformChanged(PropertyChanged);

    void SetTransformVariables(Farg<ThingData>);
    void GetTransformVariables(Shared<ThingData>) const;

    Farg<glm::vec2> Origin() const;
    Farg<glm::vec2> Scale() const;
    double Rotation(bool isDegrees = false) const;

    void Origin(Farg<glm::vec2> inOrigin);
    void Scale(Farg<glm::vec2> inScale);
    void Rotation(double inRotation, bool isDegrees = false);
};

class Transform3D
{
protected:
    enum PropertyChanged : int
    { ORIGIN, ROTATION, SCALE };

    glm::vec3 mOrigin{0.0f},
        mEuler{0.0f},
        mScale{1.0f};
    glm::quat mQuaternion{};
    RMutex mTransformMutex{};

    virtual void OnTransformChanged(PropertyChanged);

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

    void Origin(Farg<glm::vec3> inOrigin);
    void Scale(Farg<glm::vec3> inScale);
    void Quaternion(Farg<glm::quat> inQuaternion);
    void Euler(Farg<glm::vec3> inEuler, bool isDegrees = false);
};

#endif // TRANSFORM_H
