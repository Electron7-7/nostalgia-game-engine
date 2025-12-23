#ifdef FWD_DCL
    class Collider;
#elif !defined COLLIDER_H
#define COLLIDER_H

#define FWD_DCL
#   include "managers/physics_manager.hpp"
#undef  FWD_DCL

#include "device.hpp"
#include "theatre/transform_3d.hpp"

#include <Jolt/Jolt.h>
#include <Jolt/Math/Vec3.h>
#include <Jolt/Math/Quat.h>

class Collider : public Device, public Transform3D
{
public:
    Collider() = default;
    Collider(Farg<glm::vec3> Position, Farg<glm::vec3> Rotation, PhysicsBodyShape Shape);
    Collider(Farg<glm::vec3> Position, Farg<glm::vec3> Rotation, Farg<glm::vec3> Scale, PhysicsBodyShape Shape);

    void SetVariables(Farg<ThingData>) override;
    Shared<ThingData> GetVariables() const override;
    void Tick() override;

    bool BodyIDInvalid() const;
    bool Active() const;
    PhysicsBodyShape Shape() const;
    PhysicsBodyMotion Motion() const;

    void ResetTransform(bool ActivateOnReset = false) const;
    void ToggleActivation();
    void Activate() const;
    void Deactivate() const;

protected:
    JPH::Vec3 mInitialPosition{0,0,0};
    JPH::Quat mInitialRotation{JPH::Quat::sIdentity()};
    glm::vec3 mInitialImpulse{0.0f};
    PhysicsBodyShape  mShape{PhysicsBodyShape::Box};
    PhysicsBodyMotion mMotion{PhysicsBodyMotion::Static};

    JPH::BodyID& BodyID() const;
};

#endif // COLLIDER_H
