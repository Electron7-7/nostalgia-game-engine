#ifndef COLLIDER_H
#define COLLIDER_H

#include <Nostalgia/things/thinkers/3d/actor_3d.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Collision/ContactListener.h>

enum class ShapeType : ushort
{ Box, Sphere, Capsule, Cylinder, None };

enum class MotionType : ushort
{ Static, Dynamic, Kinematic, None };

struct ColliderMaterial
{
    float friction{1.0f};
};

class Collider3D : public Actor3D
{
public:
	SET_SUPER(Actor3D)
	SET_TYPEID(ThingType::Collider3D)
    virtual SIG_READY override;
    virtual SIG_SET_VARIABLES override;
    virtual SIG_GET_VARIABLES override;

    virtual void Shutdown() override;
    virtual void Tick() override;

    virtual Farg<ColliderMaterial> Material() const;
    virtual float Mass() const;
    virtual ShapeType Shape() const;
    virtual MotionType Motion() const;
    virtual bool Active() const;
    virtual bool ActivateOnNextChange() const;

    virtual void SetPosition(Farg<glm::vec3>) override;
    virtual void SetQuaternion(Farg<glm::quat>) override;
    virtual void SetRotation(Farg<glm::vec3>) override;
    virtual void SetRotationDegrees(Farg<glm::vec3>) override;
    virtual void SetScale(Farg<glm::vec3>) override;

    virtual Error SetMaterial(Farg<ColliderMaterial>);
    virtual void  SetMass(float);
    virtual Error SetShape(ShapeType, bool setActive = false);
    virtual Error SetMotion(MotionType, bool setActive = false);
    virtual void SetActive(bool) const;
    virtual void SetActivateOnNextChange(bool);

    virtual bool CreateBody(bool setActive);
    virtual void DestroyBody();

    virtual void AddImpulse(Farg<glm::vec3> inImpulse);
    virtual void AddImpulse(Farg<glm::vec3> inImpulse, Farg<glm::vec3> inPoint);
    virtual void AddAngularImpulse(Farg<glm::vec3> inAngularImpulse);
    virtual void SetLinearVelocity(Farg<glm::vec3> inLinearVelocity);

    Farg<JPH::BodyID> id() const;
    Shared<JPH::BodyCreationSettings> CreationSettings();

protected:
    friend class PhysicsEngine;
    friend class Jolt_ContactListener;
    friend class Jolt_BodyActivationListener;
    RMutex mPhysicsMutex{};
    Shared<JPH::BodyCreationSettings> m_pBodyCreationSettings{nullptr};
    JPH::BodyID mBodyID{};
    ShapeType  mShape{ShapeType::Box};
    MotionType mMotion{MotionType::Static};
    ColliderMaterial mMaterial{};
    float mMass{1.0f};
    bool mActivateOnStart{true}, mActivateOnChange{true};

    virtual void OnContactAdded(ID inOtherColliderID,
        Farg<JPH::Body> inBody1,
        Farg<JPH::Body> inBody2,
        Farg<JPH::ContactManifold> manifold,
        JPH::ContactSettings& ioSettings);
    virtual void OnContactPersisted(ID inOtherColliderID,
        Farg<JPH::Body> inBody1,
        Farg<JPH::Body> inBody2,
        Farg<JPH::ContactManifold> manifold,
        JPH::ContactSettings& ioSettings);
    // virtual void OnContactRemoved(Farg<JPH::SubShapeIDPair> inSubShapeIDPair);
    virtual void OnBodyActivated();
    virtual void OnBodyDeactivated();
};

#endif // COLLIDER_H
