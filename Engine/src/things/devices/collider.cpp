#include "collider.hpp"
#include "core/enum_prettifier.hpp"
#include "math/conversion.hpp"
#include "theatre/parser/thing_data.hpp"
#include "managers/physics_manager.hpp"
#include <Jolt/Physics/Body/BodyInterface.h>

using namespace JPH;

Error Collider::DestroyBody() const
{
    return (g_pPhysicsManager->DestroyBody(mUID))
        ? OK
        : FAILED;
}

Error Collider::CreateBody() const
{
    return (g_pPhysicsManager->CreateBody(mUID, *this, mShape, mMotion, mFriction, mDensity))
        ? OK
        : FAILED;
}

void Collider::SetVariables(Farg<ThingData> data)
{
    Device::SetVariables(data);
    SetTransformVariables(data);

    data.GetVariable(mShape, "Shape", "ColliderShape", "BodyShape");
    data.GetVariable(mMotion, "Motion", "ColliderMotion", "BodyMotion");
    data.GetVariable(mDensity, "Density", "ColliderDensity", "BodyDensity");
    data.GetVariable(mFriction, "Friction", "ColliderFriction", "BodyFriction");
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
    data->AddVariable(mDensity, "Density");
    data->AddVariable(mFriction, "Friction");
    data->AddVariable(mInitialImpulse, "InitialImpulse");
    return data;
}

void Collider::Ready()
{
    Device::Ready();
    g_pPhysicsManager->CreateBody(mUID, *this, mShape, mMotion, mDensity);
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
    mEuler = glm::eulerAngles(mQuaternion);
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
        Vec3{0,0,0},
        Vec3{0,0,0});
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

void Collider::OnTransformChanged(PropertyChanged inProp)
{
    LockGuard<RMutex> lock{mTransformMutex};
    if(BodyIDInvalid())
        { return; }
    else if(inProp == SCALE)
    {
        print_warning("Editing a Collider's scale is not allowed, yet");
        return;
    }
    g_pPhysicsManager->GetBodyInterface().SetPositionAndRotation(BodyID(),
        GlmToJolt<JPH::Vec3>(mOrigin),
        GlmToJolt<JPH::Quat>(mQuaternion),
        EActivation::DontActivate);
}

float Collider::Friction()
{
    if(BodyIDInvalid())
        { return mFriction; }
    return mFriction = g_pPhysicsManager->GetBodyInterface().GetFriction(BodyID());
}

void Collider::Friction(float inFriction)
{ g_pPhysicsManager->GetBodyInterface().SetFriction(BodyID(), mFriction = inFriction); }

float Collider::Density()
{
    if(BodyIDInvalid())
        { return mDensity; }
    return mDensity;
}

void Collider::Density(float inDensity)
{ print_debug("You can't change a Collider's density, yet"); }

PhysicsBodyShape Collider::Shape()
{ return mShape; }

void Collider::Shape(PhysicsBodyShape inShape)
{
    mShape = inShape;
    DestroyBody();
    print_error_enum(CreateBody());
}

PhysicsBodyMotion Collider::Motion()
{ return mMotion; }

void Collider::Motion(PhysicsBodyMotion inMotion)
{
    mMotion = inMotion;
    DestroyBody();
    print_error_enum(CreateBody());
}

void Collider::AddImpulse(Farg<glm::vec3> inImpulse)
{
    g_pPhysicsManager->GetBodyInterface().AddImpulse(BodyID(),
        GlmToJolt<Vec3>(inImpulse));
}

void Collider::AddImpulse(Farg<glm::vec3> inImpulse, Farg<glm::vec3> inPoint)
{
    g_pPhysicsManager->GetBodyInterface().AddImpulse(BodyID(),
        GlmToJolt<Vec3>(inImpulse),
        GlmToJolt<Vec3>(inPoint));
}

void Collider::AddAngularImpulse(Farg<glm::vec3> inAngularImpulse)
{
    g_pPhysicsManager->GetBodyInterface().AddAngularImpulse(BodyID(),
        GlmToJolt<Vec3>(inAngularImpulse));
}

void Collider::SetLinearVelocity(Farg<glm::vec3> inLinearVelocity)
{
    g_pPhysicsManager->GetBodyInterface().SetLinearVelocity(BodyID(),
        GlmToJolt<Vec3>(inLinearVelocity));
}
