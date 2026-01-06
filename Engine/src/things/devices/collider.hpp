#ifndef COLLIDER_H
#define COLLIDER_H

#include "things/devices/device.hpp"
#include "theatre/transform.hpp"
#include <glm/fwd.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>

enum class ShapeType : ushort
{ Box, Sphere, Capsule, Cylinder, None };

enum class MotionType : ushort
{ Static, Dynamic, Kinematic, None };

enum class CollisionType : ushort
{ Validated, Added, Persisted, Removed, None, };

struct ColliderMaterial
{
    float friction{1.0f};
};

class Collider : public Device, public Transform3D
{
public:
    Farg<JPH::BodyID> id() const;
    Shared<JPH::BodyCreationSettings> CreationSettings();

    virtual void OnTransformChanged(PropertyChanged) override;
    virtual void SetVariables(Farg<ThingData>) override;
    virtual Shared<ThingData> GetVariables() const override;
    virtual void Ready() override;
    virtual void Shutdown() override;
    virtual void Tick() override;

    virtual bool CreateBody(bool setActive);
    virtual void DestroyBody();

    virtual CollisionType Collision() const;
    virtual ID CollidedID() const;

    virtual Farg<ColliderMaterial> Material() const;
    virtual Error Material(Farg<ColliderMaterial>);

    virtual float Mass() const;
    virtual void  Mass(float);

    virtual ShapeType Shape() const;
    virtual Error Shape(ShapeType, bool setActive = false);

    virtual MotionType Motion() const;
    virtual Error Motion(MotionType, bool setActive = false);

    virtual bool Active() const;
    virtual void Active(bool) const;

    virtual bool SetActiveOnNextChange() const;
    virtual void SetActiveOnNextChange(bool);

    virtual void AddImpulse(Farg<glm::vec3> inImpulse);
    virtual void AddImpulse(Farg<glm::vec3> inImpulse, Farg<glm::vec3> inPoint);
    virtual void AddAngularImpulse(Farg<glm::vec3> inAngularImpulse);
    virtual void SetLinearVelocity(Farg<glm::vec3> inLinearVelocity);

protected:
    RMutex mPhysicsMutex{};
    Shared<JPH::BodyCreationSettings> m_pBodyCreationSettings{nullptr};
    JPH::BodyID mBodyID{};
    ShapeType  mShape{ShapeType::Box};
    MotionType mMotion{MotionType::Static};
    ColliderMaterial mMaterial{};
    float mMass{1.0f};
    bool mActivateOnNextChange{true};
    CollisionType mCurrentCollision{CollisionType::None};
    ID mCurrentCollisionID{};

    friend void s_NotifyCollider(CollisionType,ID,ID);
    void collision_notification(CollisionType, ID inOtherColliderID);
};

#endif // COLLIDER_H
