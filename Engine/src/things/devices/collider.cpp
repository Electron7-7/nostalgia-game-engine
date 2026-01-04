#include "collider.hpp"
#include "core/enum_prettifier.hpp"
#include "math/conversion.hpp"
#include "theatre/parser/thing_data.hpp"
#include "managers/physics_manager.hpp"
#include <Jolt/Physics/Body/BodyInterface.h>

using namespace JPH;

Collider::Collider(const glm::vec3& pos, const glm::vec3& rot, PhysicsBodyShape shape):
    Collider(pos, rot, glm::vec3(1.0f), shape) {}

Collider::Collider(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scale, PhysicsBodyShape shape):
    mShape(shape)
{
    mOrigin = pos;
    mEuler  = rot;
    mScale  = scale;
}

void Collider::SetVariables(Farg<ThingData> data)
{
    Device::SetVariables(data);
    SetTransformVariables(data);

    data.GetVariable(mShape, "Shape", "ColliderShape", "BodyShape");
    data.GetVariable(mMotion, "Motion", "ColliderMotion", "BodyMotion");
    data.GetVariable(mInitialImpulse, "InitialImpulse", "Impulse");

    GlmToJolt(mOrigin, mInitialPosition);
    GlmToJolt(mQuaternion, mInitialRotation);
}

Shared<ThingData> Collider::GetVariables() const
{
    Shared<ThingData> data{Device::GetVariables()};
    GetTransformVariables(data);
    data->AddVariable(mShape, "Shape");
    data->AddVariable(mMotion, "Motion");
    data->AddVariable(mInitialImpulse, "InitialImpulse");
    return data;
}

void Collider::Ready()
{
    g_pPhysicsManager->CreateBody(mUID, *this, mShape, mMotion);
    print_jolt("Body Created - Shape: {}, Motion: {}",
        EnumPrettifier::Get(mShape, EnumSet::PhysicsBodyShape),
        EnumPrettifier::Get(mMotion, EnumSet::PhysicsBodyMotion));
}

void Collider::Shutdown()
{
    g_pPhysicsManager->DestroyBody(mUID);
    print_jolt("Body Destroyed - Shape: {}, Motion: {}",
        EnumPrettifier::Get(mShape, EnumSet::PhysicsBodyShape),
        EnumPrettifier::Get(mMotion, EnumSet::PhysicsBodyMotion));
    Device::Shutdown();
}

void Collider::Tick()
{
    if(BodyIDInvalid())
        { return; }
    JoltToGlm(g_pPhysicsManager->GetBodyInterface().GetRotation(BodyID()), mQuaternion);
    JoltToGlm(g_pPhysicsManager->GetBodyInterface().GetCenterOfMassPosition(BodyID()), mOrigin);
}

bool Collider::Active() const
{ return g_pPhysicsManager->GetBodyInterface().IsActive(BodyID()); }

PhysicsBodyShape Collider::Shape() const
{ return mShape; }

PhysicsBodyMotion Collider::Motion() const
{ return mMotion; }

void Collider::ResetTransform(bool activate) const
{
    if(BodyIDInvalid())
        { return; }
    g_pPhysicsManager->GetBodyInterface().SetPositionRotationAndVelocity(
        BodyID(),
        mInitialPosition,
        mInitialRotation,
        JPH::Vec3{0,0,0},
        JPH::Vec3{0,0,0});
    if(activate)
        { Activate(); }
}

void Collider::ToggleActivation()
{
    if(BodyIDInvalid())
        { return; }
    else if(Active())
        { Activate(); }
    else
        { Deactivate(); }
}

void Collider::Activate() const
{ g_pPhysicsManager->GetBodyInterface().ActivateBody(BodyID()); }

void Collider::Deactivate() const
{ g_pPhysicsManager->GetBodyInterface().DeactivateBody(BodyID()); }

bool Collider::BodyIDInvalid() const
{ return BodyID().IsInvalid(); }

JPH::BodyID& Collider::BodyID() const
{ return g_pPhysicsManager->GetBodyID(mUID); }

void Collider::Origin(Farg<glm::vec3> inOrigin)
{
    LockGuard<RMutex> lock{mTransformMutex};
    mOrigin = inOrigin;
    if(BodyIDInvalid())
        { return; }
    g_pPhysicsManager->GetBodyInterface().SetPosition(BodyID(),
        GlmToJolt<JPH::Vec3>(mOrigin),
        EActivation::DontActivate);
}

void Collider::Scale(Farg<glm::vec3> inScale)
{ print_warning("Currently unable to change collider scale"); }

void Collider::Quaternion(Farg<glm::quat> inQuaternion)
{
    LockGuard<RMutex> lock{mTransformMutex};
    mQuaternion = inQuaternion;
    mEuler = glm::eulerAngles(mQuaternion);
    if(BodyIDInvalid())
        { return; }
    g_pPhysicsManager->GetBodyInterface().SetRotation(BodyID(),
        GlmToJolt<JPH::Quat>(mQuaternion),
        EActivation::DontActivate);
}

void Collider::Euler(Farg<glm::vec3> inEuler, bool isDegrees)
{
    LockGuard<RMutex> lock{mTransformMutex};
    mEuler = (isDegrees) ? glm::radians(inEuler) : inEuler;
    mQuaternion = glm::quat{mEuler};
    if(BodyIDInvalid())
        { return; }
    g_pPhysicsManager->GetBodyInterface().SetRotation(BodyID(),
        GlmToJolt<JPH::Quat>(glm::quat{(isDegrees) ? glm::radians(inEuler) : inEuler}),
        EActivation::DontActivate);
}
