#include "collider.hpp"
#include "math/conversion.hpp"
#include "theatre_parser/thing_data.hpp"
#include "managers/physics_manager.hpp"

using namespace JPH;

Collider::Collider(const glm::vec3& pos, const glm::vec3& rot, const penum_t& shape):
    Collider(pos, rot, glm::vec3(1.0f), shape) {}

Collider::Collider(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scale, const penum_t& shape):
    mShape(shape)
{
    mOrigin = pos;
    mEuler  = rot;
    mScale  = scale;
}

void Collider::SetVariables(const ThingData& data)
{
    Device::SetVariables(data);
    SetTransformVariables(data);

    data.GetVariable(mShape, "Shape", "ColliderShape", "BodyShape");
    data.GetVariable(mMotion, "Motion", "ColliderMotion", "BodyMotion");
    data.GetVariable(mInitialImpulse, "InitialImpulse", "Impulse");

    GlmToJolt(mOrigin, mInitialPosition);
    GlmToJolt(mQuaternion, mInitialRotation);
}

ThingData Collider::GetVariables() const
{
    ThingData data{Device::GetVariables()};
    GetTransformVariables(data);
    data.AddVariable(mShape, "Shape");
    data.AddVariable(mMotion, "Motion");
    data.AddVariable(mInitialImpulse, "InitialImpulse");
    return data;
}

void Collider::Tick()
{
    if(BodyIDInvalid())
        { return; }
    JoltToGlm(
        g_pPhysicsManager->GetBodyInterface().GetRotation(BodyID()),
        mQuaternion);
    JoltToGlm(
        g_pPhysicsManager->GetBodyInterface().GetCenterOfMassPosition(BodyID()),
        mOrigin);
}

bool Collider::Active() const
{ return g_pPhysicsManager->GetBodyInterface().IsActive(BodyID()); }

const penum_t& Collider::Shape() const
{ return mShape; }

const penum_t& Collider::Motion() const
{ return mMotion; }

void Collider::ResetTransform(bool activate) const
{
    if(BodyIDInvalid())
        { return; }
    g_pPhysicsManager->GetBodyInterface().SetPositionRotationAndVelocity(
        BodyID(),
        mInitialPosition,
        mInitialRotation,
        JPH::Vec3(0, 0, 0),
        JPH::Vec3(0, 0, 0));
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
