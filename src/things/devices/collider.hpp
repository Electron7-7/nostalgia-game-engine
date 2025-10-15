#ifndef COLLIDER_H
#define COLLIDER_H

#include "device.hpp"
#include "world/transform_3d.hpp"

#include <physics/framework.hpp>
#include <glm/glm.hpp>

class Collider : public Device, public Transform3D
{
public:
    Collider() = default;
    Collider(const glm::vec3& Position, const glm::vec3& Rotation, const penum_t& Shape);
    Collider(const glm::vec3& Position, const glm::vec3& Rotation, const glm::vec3& Scale, const penum_t& Shape);

    ~Collider();

    void SetVariables(const ThingData&);
    ThingData GetVariables() const;
    void Tick();

    bool BodyIDInvalid() const;
    bool Active() const;
    const penum_t& Shape() const;
    const penum_t& Motion() const;

    void ResetTransform() const;
    void ToggleActivation();

protected:
    JPH::RVec3 mInitialPosition{JPH::Vec3()};
    JPH::Quat  mInitialRotation{JPH::Quat::sIdentity()};
    glm::vec3  mInitialImpulse{0.0f};
    penum_t    mShape{PhysicsBodyShape::Box};
    penum_t    mMotion{PhysicsBodyMotion::Static};

    JPH::BodyID& BodyID() const;
};

#endif // COLLIDER_H
