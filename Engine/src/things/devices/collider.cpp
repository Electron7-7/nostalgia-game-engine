#include "collider.hpp"
#include "core/enum_prettifier.hpp" // IWYU pragma: keep
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
    { return new BoxShapeSettings{inScale}; }
template<>
    const ShapeSettings* sMakeShapeSettings<ShapeType::Sphere>(Vec3Arg inScale)
    { return new SphereShapeSettings{inScale.GetX() * 0.5f}; }
template<>
    const ShapeSettings* sMakeShapeSettings<ShapeType::Cylinder>(Vec3Arg inScale)
    { return new CylinderShapeSettings{inScale.GetZ() * 0.5f, inScale.GetX() * 0.5f}; }
template<>
    const ShapeSettings* sMakeShapeSettings<ShapeType::Capsule>(Vec3Arg inScale)
    { return new CapsuleShapeSettings{inScale.GetZ() * 0.5f, inScale.GetX() * 0.5f}; }

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

void Collider::SetVariables(Farg<ThingData> data)
{
    Device::SetVariables(data);
    SetTransformVariables(data);

    data.GetVariable(mShape, "Shape", "ColliderShape", "BodyShape");
    data.GetVariable(mMotion, "Motion", "ColliderMotion", "BodyMotion");
    data.GetVariable(mMass, "Mass", "ColliderMass", "BodyMass");
    data.GetVariable(mMaterial.friction, "Friction", "ColliderFriction", "BodyFriction");
    data.GetVariable(mStartInactive, "StartInactive", "Inactive");
    if(data.GetVariable(mStartInactive, "StartActive", "Active"))
        { mStartInactive = false; }

    if(CreateBody(!mStartInactive))
    {
        print_jolt("Body Created - Shape: {}, Motion: {}",
            PRETTIFY_ENUM(mShape, ShapeType),
            PRETTIFY_ENUM(mMotion, MotionType));
    }
    else
        { print_error("Failed to create physics body for Collider#{}", mUID[]); }
}

Shared<ThingData> Collider::GetVariables() const
{
    Shared<ThingData> data{Device::GetVariables()};
    GetTransformVariables(data);
    data->AddVariable(mShape, "Shape");
    data->AddVariable(mMotion, "Motion");
    data->AddVariable(mMass, "Mass");
    data->AddVariable(mMaterial.friction, "Friction");
    data->AddVariable(mStartInactive, "Inactive");
    return data;
}

void Collider::Ready()
{
    Device::Ready();
}

void Collider::Shutdown()
{
    DestroyBody();
    print_jolt("Body Destroyed - Shape: {}, Motion: {}",
        PRETTIFY_ENUM(mShape, ShapeType),
        PRETTIFY_ENUM(mMotion, MotionType));
    Device::Shutdown();
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
            // GlmToJolt<Quat>(mQuaternion),
            Quat::sEulerAngles(GlmToJolt<Vec3>(mEuler)),
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
    return !mBodyID.IsInvalid();
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

void Collider::OnTransformChanged(PropertyChanged inProp)
{
    ASSERT_BODYID()
    LockGuard<RMutex> lock{mTransformMutex};
    if(inProp == SCALE)
    {
        print_warning("Editing a Collider's scale is not allowed, yet");
        return;
    }
    PhysicsEngine::Instance()->BodyInterface().SetPositionAndRotation(BodyID(),
        GlmToJolt<Vec3>(mOrigin),
        GlmToJolt<Quat>(mQuaternion),
        EActivation::DontActivate);
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
    PhysicsEngine::Instance()->BodyInterface().AddImpulse(BodyID(),
        GlmToJolt<Vec3>(inImpulse));
}

void Collider::AddImpulse(Farg<glm::vec3> inImpulse, Farg<glm::vec3> inPoint)
{
    ASSERT_BODYID()
    PhysicsEngine::Instance()->BodyInterface().AddImpulse(BodyID(),
        GlmToJolt<Vec3>(inImpulse),
        GlmToJolt<Vec3>(inPoint));
}

void Collider::AddAngularImpulse(Farg<glm::vec3> inAngularImpulse)
{
    ASSERT_BODYID()
    PhysicsEngine::Instance()->BodyInterface().AddAngularImpulse(BodyID(),
        GlmToJolt<Vec3>(inAngularImpulse));
}

void Collider::SetLinearVelocity(Farg<glm::vec3> inLinearVelocity)
{
    ASSERT_BODYID()
    PhysicsEngine::Instance()->BodyInterface().SetLinearVelocity(BodyID(),
        GlmToJolt<Vec3>(inLinearVelocity));
}
