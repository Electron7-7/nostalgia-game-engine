#include "collider3d.hpp"
#include "core/enum_prettifier.hpp"
#include "physics/engine.hpp"
#include "math/conversion.hpp"
#include "theatre/parser/thing_data.hpp"
#include "thirdparty/frozen/map.h"
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>

bool gEnableMsg_ContactValidate{false},
    gEnableMsg_ContactAdded{false},
    gEnableMsg_ContactPersisted{false},
    gEnableMsg_ContactRemoved{false},
    gEnableMsg_BodyActivated{false},
    gEnableMsg_BodyDeactivated{false};

#define ASSERT_BODYID(RETURN_VALUE...) \
    if(mBodyID.IsInvalid()) { return RETURN_VALUE; }

using namespace JPH;

typedef const ShapeSettings* (*ShapeSettingsMaker_f)(Vec3Arg);

template<ShapeType Shape>
    const ShapeSettings* sMakeShapeSettings(Vec3Arg inScale);

template<>
    const ShapeSettings* sMakeShapeSettings<ShapeType::Box>(Vec3Arg inScale)
    { return new BoxShapeSettings(inScale); }
template<>
    const ShapeSettings* sMakeShapeSettings<ShapeType::Sphere>(Vec3Arg inScale)
    { return new SphereShapeSettings(inScale.GetX()); }
template<>
    const ShapeSettings* sMakeShapeSettings<ShapeType::Cylinder>(Vec3Arg inScale)
    { return new CylinderShapeSettings(inScale.GetZ(), inScale.GetX()); }
template<>
    const ShapeSettings* sMakeShapeSettings<ShapeType::Capsule>(Vec3Arg inScale)
    { return new CapsuleShapeSettings(inScale.GetZ(), inScale.GetX()); }

static constexpr frozen::map<ShapeType, ShapeSettingsMaker_f, 5>
s_ShapeSettingsMakers
{
    { ShapeType::None,     &sMakeShapeSettings<ShapeType::Box>      },
    { ShapeType::Box,      &sMakeShapeSettings<ShapeType::Box>      },
    { ShapeType::Sphere,   &sMakeShapeSettings<ShapeType::Sphere>   },
    { ShapeType::Cylinder, &sMakeShapeSettings<ShapeType::Cylinder> },
    { ShapeType::Capsule,  &sMakeShapeSettings<ShapeType::Capsule>  },
};

static constexpr EActivation s_Activation(bool setActive) noexcept
{
    return (setActive)
        ? EActivation::Activate
        : EActivation::DontActivate;
}

Farg<JPH::BodyID> Collider::id() const
{ return mBodyID; }

Shared<JPH::BodyCreationSettings> Collider::CreationSettings()
{ return m_pBodyCreationSettings; }

void Collider::SetVariables(Farg<ThingData> data)
{
    Actor3D::SetVariables(data);
    SetTransformVariables(data);

    data.GetVariable(mShape, "Shape", "ColliderShape", "BodyShape");
    data.GetVariable(mMotion, "Motion", "ColliderMotion", "BodyMotion");
    data.GetVariable(mMass, "Mass", "ColliderMass", "BodyMass");
    data.GetVariable(mMaterial.friction, "Friction", "ColliderFriction", "BodyFriction");
    data.GetVariable(mMaterial.friction, "Friction", "ColliderFriction", "BodyFriction");
    data.GetVariable(mActivateOnNextChange, "StartActive", "Active");
    if(data.GetVariable(mActivateOnNextChange, "StartInactive", "Inactive"))
        { mActivateOnNextChange = true; }

    if(CreateBody(mActivateOnNextChange))
    {
        print_jolt("Physics Body Created [{}, {}]",
            EnumPrettifier::Prettify(mShape),
            EnumPrettifier::Prettify(mMotion));
    }
    else
        { print_error("Failed to create physics body for Collider#{}", mUID[]); }

    mActivateOnNextChange = false;
}

Shared<ThingData> Collider::GetVariables() const
{
    Shared<ThingData> data{Actor3D::GetVariables()};
    GetTransformVariables(data);
    data->AddVariable(mShape, "Shape");
    data->AddVariable(mMotion, "Motion");
    data->AddVariable(mMass, "Mass");
    data->AddVariable(mMaterial.friction, "Friction");
    data->AddVariable(mActivateOnNextChange, "StartActive");
    return data;
}

void Collider::Ready()
{
    Actor3D::Ready();
}

void Collider::Shutdown()
{
    DestroyBody();
    print_jolt("Physics Body Destroyed [{}, {}]",
        EnumPrettifier::Prettify(mShape),
        EnumPrettifier::Prettify(mMotion));
    Actor3D::Shutdown();
}

void Collider::Tick()
{
    ASSERT_BODYID()
    JoltToGlm(PhysicsEngine::Instance()->BodyInterface().GetRotation(mBodyID), mQuaternion);
    JoltToGlm(PhysicsEngine::Instance()->BodyInterface().GetCenterOfMassPosition(mBodyID), mOrigin);
    mEuler = glm::eulerAngles(mQuaternion);
}

bool Collider::CreateBody(bool setActive)
{
    m_pBodyCreationSettings =
        MakeShared<BodyCreationSettings>(s_ShapeSettingsMakers.at(mShape)(GlmToJolt<Vec3>(mScale)),
            GlmToJolt<Vec3>(mOrigin),
            GlmToJolt<Quat>(mQuaternion),
            PhysicsEngine::ConvertMotionType(mMotion),
            PhysicsEngine::GetObjectLayer(mMotion));

    m_pBodyCreationSettings->mOverrideMassProperties =
        EOverrideMassProperties::CalculateInertia;

    m_pBodyCreationSettings->mMassPropertiesOverride.mMass = mMass;
    m_pBodyCreationSettings->mFriction = mMaterial.friction;

#pragma message("TODO: Add bodies in a batch and activate them in a batch")
    mBodyID = PhysicsEngine::Instance()
        ->BodyInterface()
            .CreateAndAddBody(*m_pBodyCreationSettings, s_Activation(setActive));
    if(mBodyID.IsInvalid())
        { return false; }
    PhysicsEngine::Instance()->BodyInterface().SetUserData(mBodyID, mUID[]);
    return true;
}

void Collider::DestroyBody()
{
    if(mBodyID.IsInvalid())
        { print_warning("mBodyID.IsInvalid == true (body was already destroyed?)"); }
    PhysicsEngine::Instance()->BodyInterface().RemoveBody(mBodyID);
    PhysicsEngine::Instance()->BodyInterface().DestroyBody(mBodyID);
    mBodyID = BodyID{BodyID::cInvalidBodyID};
}

ShapeType Collider::Shape() const
{ return mShape; }

Error Collider::Shape(ShapeType inShape, bool isActive)
{
    ASSERT_BODYID(ERR_INVALID_ID)
    mShape = inShape;
    PhysicsEngine::Instance()->BodyInterface().SetShape(mBodyID,
        s_ShapeSettingsMakers.at(mShape)(GlmToJolt<Vec3>(mScale))->Create().Get(),
        true,
        s_Activation(isActive));
    PhysicsEngine::Instance()->BodyInterface().NotifyShapeChanged(mBodyID,
        GlmToJolt<Vec3>(mOrigin),
        true,
        s_Activation(isActive));
    return OK;
}

MotionType Collider::Motion() const
{ return mMotion; }

Error Collider::Motion(MotionType inMotion, bool isActive)
{
    ASSERT_BODYID(ERR_INVALID_ID)
    mMotion = inMotion;
    PhysicsEngine::Instance()->BodyInterface().SetShape(mBodyID,
        s_ShapeSettingsMakers.at(mShape)(GlmToJolt<Vec3>(mScale))->Create().Get(),
        true,
        s_Activation(isActive));
    PhysicsEngine::Instance()->BodyInterface().NotifyShapeChanged(mBodyID,
        GlmToJolt<Vec3>(mOrigin),
        true,
        s_Activation(isActive));
    return OK;
}

bool Collider::Active() const
{
    ASSERT_BODYID(false)
    return PhysicsEngine::Instance()->BodyInterface().IsActive(mBodyID);
}

void Collider::Active(bool isActive) const
{
    ASSERT_BODYID()
    if(isActive)
        { PhysicsEngine::Instance()->BodyInterface().ActivateBody(mBodyID); }
    else
        { PhysicsEngine::Instance()->BodyInterface().DeactivateBody(mBodyID); }
}

bool Collider::SetActiveOnNextChange() const
{ return mActivateOnNextChange; }

void Collider::SetActiveOnNextChange(bool setActive)
{ mActivateOnNextChange = setActive; }

void Collider::OnTransformChanged(PropertyChanged inProp)
{
    ASSERT_BODYID()
    LockGuard<RMutex> lock{mTransformMutex};
    if(inProp == SCALE)
    {
        print_warning("Editing a Collider's scale is not allowed, yet");
        return;
    }
    PhysicsEngine::Instance()->BodyInterface().SetPositionAndRotation(mBodyID,
        GlmToJolt<Vec3>(mOrigin),
        GlmToJolt<Quat>(mQuaternion),
        s_Activation(mActivateOnNextChange));
}

Farg<ColliderMaterial> Collider::Material() const
{ return mMaterial; }

Error Collider::Material(Farg<ColliderMaterial> inMaterial)
{
    ASSERT_BODYID(ERR_INVALID_ID)
    mMaterial = inMaterial;
    PhysicsEngine::Instance()->BodyInterface().SetFriction(mBodyID,
        mMaterial.friction);
    return OK;
}

float Collider::Mass() const
{
    ASSERT_BODYID(mMass)
    return mMass;
}

void Collider::Mass(float inMass)
{ print_debug("You can't change a Collider's mass, yet"); }

void Collider::AddImpulse(Farg<glm::vec3> inImpulse)
{
    ASSERT_BODYID()
    PhysicsEngine::Instance()->BodyInterface().AddImpulse(mBodyID,
        GlmToJolt<Vec3>(inImpulse));
}

void Collider::AddImpulse(Farg<glm::vec3> inImpulse, Farg<glm::vec3> inPoint)
{
    ASSERT_BODYID()
    PhysicsEngine::Instance()->BodyInterface().AddImpulse(mBodyID,
        GlmToJolt<Vec3>(inImpulse),
        GlmToJolt<Vec3>(inPoint));
}

void Collider::AddAngularImpulse(Farg<glm::vec3> inAngularImpulse)
{
    ASSERT_BODYID()
    PhysicsEngine::Instance()->BodyInterface().AddAngularImpulse(mBodyID,
        GlmToJolt<Vec3>(inAngularImpulse));
}

void Collider::SetLinearVelocity(Farg<glm::vec3> inLinearVelocity)
{
    ASSERT_BODYID()
    PhysicsEngine::Instance()->BodyInterface().SetLinearVelocity(mBodyID,
        GlmToJolt<Vec3>(inLinearVelocity));
}
