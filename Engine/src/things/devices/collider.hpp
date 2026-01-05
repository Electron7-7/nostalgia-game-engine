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
    virtual void OnTransformChanged(PropertyChanged) override;
    virtual void SetVariables(Farg<ThingData>) override;
    virtual Shared<ThingData> GetVariables() const override;
    virtual void Ready() override;
    virtual void Shutdown() override;
    virtual void Tick() override;

    virtual float Friction();
    virtual void  Friction(float);
    virtual float Density();
    virtual void  Density(float);
    virtual PhysicsBodyShape Shape();
    virtual void Shape(PhysicsBodyShape);
    virtual PhysicsBodyMotion Motion();
    virtual void Motion(PhysicsBodyMotion);

    virtual void AddImpulse(Farg<glm::vec3> inImpulse);
    virtual void AddImpulse(Farg<glm::vec3> inImpulse, Farg<glm::vec3> inPoint);
    virtual void AddAngularImpulse(Farg<glm::vec3> inAngularImpulse);
    virtual void SetLinearVelocity(Farg<glm::vec3> inLinearVelocity);

    bool BodyIDInvalid() const;
    bool Active() const;
    PhysicsBodyShape Shape() const;
    PhysicsBodyMotion Motion() const;

    void ResetTransform(bool ActivateOnReset = false) const;
    void ToggleActivation();
    void Activate() const;
    void Deactivate() const;

protected:
    Error DestroyBody() const;
    Error CreateBody() const;

    JPH::Vec3 mInitialPosition{0,0,0};
    JPH::Quat mInitialRotation{JPH::Quat::sIdentity()};
    glm::vec3 mInitialImpulse{0.0f};
    float mDensity{0.0f};
    float mFriction{0.0f};
    PhysicsBodyShape  mShape{PhysicsBodyShape::Box};
    PhysicsBodyMotion mMotion{PhysicsBodyMotion::Static};

    JPH::BodyID& BodyID() const;
};

#endif // COLLIDER_H
