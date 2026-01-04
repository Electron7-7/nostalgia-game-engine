#ifndef COLLIDER_H
#define COLLIDER_H

#include "fwd/physics.hpp"
#include "things/devices/device.hpp"
#include "theatre/transform.hpp"

enum class PhysicsBodyShape : ushort
{ Box = 0, Sphere = 1, Capsule = 2, Cylinder = 3, None = 4 };

enum class PhysicsBodyMotion : ushort
{ Static = 0, Dynamic = 1, Kinematic = 2, None = 3 };

class Collider : public Device, public Transform3D
{
public:
    Collider() = default;
    Collider(Farg<glm::vec3> Position, Farg<glm::vec3> Rotation, PhysicsBodyShape Shape);
    Collider(Farg<glm::vec3> Position, Farg<glm::vec3> Rotation, Farg<glm::vec3> Scale, PhysicsBodyShape Shape);

    virtual void SetVariables(Farg<ThingData>) override;
    virtual Shared<ThingData> GetVariables() const override;
    virtual void Ready() override;
    virtual void Shutdown() override;
    virtual void Tick() override;

    virtual void Origin(Farg<glm::vec3> inOrigin) override;
    virtual void Scale(Farg<glm::vec3> inScale) override;
    virtual void Quaternion(Farg<glm::quat> inQuaternion) override;
    virtual void Euler(Farg<glm::vec3> inEuler, bool isDegrees = false) override;

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
