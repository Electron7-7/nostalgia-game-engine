#include "./collider_3d.hpp"
#include "physics/engine.hpp"
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include "theatre/theatre.hpp"
#include "thirdparty/frozen/map.h"

using namespace Math;
using namespace TheatreFile;

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

Farg<JPH::BodyID> Collider3D::id() const
{ return mBodyID; }

Shared<JPH::BodyCreationSettings> Collider3D::CreationSettings()
{ return m_pBodyCreationSettings; }

void Collider3D::SetVariables(Farg<ThingData> data)
{
    Actor3D::SetVariables(data);

    data.get_variable(mShape, "Shape", "ColliderShape", "BodyShape");
    data.get_variable(mMotion, "Motion", "ColliderMotion", "BodyMotion");
    data.get_variable(mMass, "Mass", "ColliderMass", "BodyMass");
    data.get_variable(mMaterial.friction, "Friction", "ColliderFriction", "BodyFriction");
    data.get_variable(mActivateOnNextChange, "StartActive", "Active");
    if(data.get_variable(mActivateOnNextChange, "StartInactive", "Inactive") == OK)
        { mActivateOnNextChange = true; }

    if(CreateBody(mActivateOnNextChange))
    {
        print_jolt("Physics Body Created [{}, {}]",
            EnumPrettifier::Prettify(mShape),
            EnumPrettifier::Prettify(mMotion));
    }
    else
        { print_error("Failed to create physics body for Collider3D#{}", mUID()); }

    mActivateOnNextChange = false;
}

Shared<ThingData> Collider3D::GetVariables() const
{
    Shared<ThingData> data{Actor3D::GetVariables()};
    data->set_variable(mShape, "Shape");
    data->set_variable(mMotion, "Motion");
    data->set_variable(mMass, "Mass");
    data->set_variable(mMaterial.friction, "Friction");
    data->set_variable(mActivateOnNextChange, "StartActive");
    return data;
}

void Collider3D::Ready()
{
    Actor3D::Ready();
}

void Collider3D::Shutdown()
{
    DestroyBody();
    print_jolt("Physics Body Destroyed [{}, {}]",
        EnumPrettifier::Prettify(mShape),
        EnumPrettifier::Prettify(mMotion));
    Actor3D::Shutdown();
}

void Collider3D::Tick()
{
    ASSERT_BODYID()

    // Avoid updating global transforms every tick by only updating rotation and position when they don't match
    // with the Jolt physics body's rotation and position.
    auto new_quat{Convert<glm::quat>(PhysicsEngine::Instance()
        ->BodyInterface()
            .GetRotation(mBodyID))};
    auto new_pos{Convert<glm::vec3>(PhysicsEngine::Instance()
        ->BodyInterface()
            .GetCenterOfMassPosition(mBodyID))};
    if(new_quat != mLocalTransform.quaternion)
        { Actor3D::SetQuaternion(new_quat); }
    if(new_pos != mLocalTransform.position)
        { Actor3D::SetPosition(new_pos); }
}

Farg<ColliderMaterial> Collider3D::Material() const
{ return mMaterial; }

float Collider3D::Mass() const
{
    ASSERT_BODYID(mMass)
    return mMass;
}

ShapeType Collider3D::Shape() const
{ return mShape; }

MotionType Collider3D::Motion() const
{ return mMotion; }

bool Collider3D::Active() const
{
    ASSERT_BODYID(false)
    return PhysicsEngine::Instance()->BodyInterface().IsActive(mBodyID);
}

bool Collider3D::ActivateOnNextChange() const
{ return mActivateOnNextChange; }

void Collider3D::SetPosition(Farg<glm::vec3> inPosition)
{
    Actor3D::SetPosition(inPosition);
    PhysicsEngine::Instance()->BodyInterface().SetPositionAndRotationWhenChanged(mBodyID,
        Math::Convert<Vec3>(mLocalTransform.position),
        Math::Convert<Quat>(mLocalTransform.quaternion),
        PhysicsEngine::GetActivation(mActivateOnNextChange));
    _update_global_transform();
}

void Collider3D::SetQuaternion(Farg<glm::quat> inQuaternion)
{
    Actor3D::SetQuaternion(inQuaternion);
    PhysicsEngine::Instance()->BodyInterface().SetPositionAndRotationWhenChanged(mBodyID,
        Math::Convert<Vec3>(mLocalTransform.position),
        Math::Convert<Quat>(mLocalTransform.quaternion),
        PhysicsEngine::GetActivation(mActivateOnNextChange));
    _update_global_transform();
}

void Collider3D::SetRotation(Farg<glm::vec3> inRotation)
{
    Actor3D::SetRotation(inRotation);
    PhysicsEngine::Instance()->BodyInterface().SetPositionAndRotationWhenChanged(mBodyID,
        Math::Convert<Vec3>(mLocalTransform.position),
        Math::Convert<Quat>(mLocalTransform.quaternion),
        PhysicsEngine::GetActivation(mActivateOnNextChange));
    _update_global_transform();
}

void Collider3D::SetRotationDegrees(Farg<glm::vec3> inRotation)
{
    Actor3D::SetRotationDegrees(inRotation);
    PhysicsEngine::Instance()->BodyInterface().SetPositionAndRotationWhenChanged(mBodyID,
        Math::Convert<Vec3>(mLocalTransform.position),
        Math::Convert<Quat>(mLocalTransform.quaternion),
        PhysicsEngine::GetActivation(mActivateOnNextChange));
    _update_global_transform();
}

void Collider3D::SetScale(Farg<glm::vec3> inScale)
{
    Actor3D::SetScale(inScale);
    PhysicsEngine::Instance()->BodyInterface().SetPositionAndRotationWhenChanged(mBodyID,
        Math::Convert<Vec3>(mLocalTransform.position),
        Math::Convert<Quat>(mLocalTransform.quaternion),
        PhysicsEngine::GetActivation(mActivateOnNextChange));
    _update_global_transform();
}

Error Collider3D::SetMaterial(Farg<ColliderMaterial> inMaterial)
{
    ASSERT_BODYID(ERR_INVALID_ID)
    mMaterial = inMaterial;
    PhysicsEngine::Instance()->BodyInterface().SetFriction(mBodyID,
        mMaterial.friction);
    return OK;
}

void Collider3D::SetMass(float inMass)
{ print_debug("You can't change a Collider3D's mass, yet"); }

Error Collider3D::SetShape(ShapeType inShape, bool isActive)
{
    ASSERT_BODYID(ERR_INVALID_ID)
    mShape = inShape;
    PhysicsEngine::Instance()->BodyInterface().SetShape(mBodyID,
        s_ShapeSettingsMakers.at(mShape)(Math::Convert<Vec3>(mLocalTransform.scale))->Create().Get(),
        true,
        PhysicsEngine::GetActivation(isActive));
    PhysicsEngine::Instance()->BodyInterface().NotifyShapeChanged(mBodyID,
        Math::Convert<Vec3>(mLocalTransform.position),
        true,
        PhysicsEngine::GetActivation(isActive));
    return OK;
}

Error Collider3D::SetMotion(MotionType inMotion, bool isActive)
{
    ASSERT_BODYID(ERR_INVALID_ID)
    mMotion = inMotion;
    PhysicsEngine::Instance()->BodyInterface().SetShape(mBodyID,
        s_ShapeSettingsMakers.at(mShape)(Math::Convert<Vec3>(mLocalTransform.scale))->Create().Get(),
        true,
        PhysicsEngine::GetActivation(isActive));
    PhysicsEngine::Instance()->BodyInterface().NotifyShapeChanged(mBodyID,
        Math::Convert<Vec3>(mLocalTransform.position),
        true,
        PhysicsEngine::GetActivation(isActive));
    return OK;
}

void Collider3D::SetActive(bool isActive) const
{
    ASSERT_BODYID()
    if(isActive)
        { PhysicsEngine::Instance()->BodyInterface().ActivateBody(mBodyID); }
    else
        { PhysicsEngine::Instance()->BodyInterface().DeactivateBody(mBodyID); }
}

void Collider3D::SetActivateOnNextChange(bool setActive)
{ mActivateOnNextChange = setActive; }

bool Collider3D::CreateBody(bool setActive)
{
    m_pBodyCreationSettings =
        MakeShared<BodyCreationSettings>(
            s_ShapeSettingsMakers.at(mShape)(Math::Convert<Vec3>(mLocalTransform.scale)),
            Math::Convert<Vec3>(mLocalTransform.position),
            Math::Convert<Quat>(mLocalTransform.quaternion),
            PhysicsEngine::ConvertMotionType(mMotion),
            PhysicsEngine::GetObjectLayer(mMotion));

    m_pBodyCreationSettings->mOverrideMassProperties =
        EOverrideMassProperties::CalculateInertia;

    m_pBodyCreationSettings->mMassPropertiesOverride.mMass = mMass;
    m_pBodyCreationSettings->mFriction = mMaterial.friction;

    auto engine{PhysicsEngine::Instance()};

    mBodyID = engine->BodyInterface().CreateAndAddBody(*m_pBodyCreationSettings,
        engine->GetActivation(mActivateOnNextChange));

    if(mBodyID.IsInvalid())
        { return false; }

    engine->BodyInterface().SetUserData(mBodyID, mUID());
    return true;
}

void Collider3D::DestroyBody()
{
    if(mBodyID.IsInvalid())
        { print_warning("mBodyID.IsInvalid == true (body was already destroyed?)"); }
    auto engine{PhysicsEngine::Instance()};
    engine->BodyInterface().RemoveBody(mBodyID);
    engine->BodyInterface().DestroyBody(mBodyID);
}

void Collider3D::AddImpulse(Farg<glm::vec3> inImpulse)
{
    ASSERT_BODYID()
    PhysicsEngine::Instance()->BodyInterface().AddImpulse(mBodyID,
        Math::Convert<Vec3>(inImpulse));
}

void Collider3D::AddImpulse(Farg<glm::vec3> inImpulse, Farg<glm::vec3> inPoint)
{
    ASSERT_BODYID()
    PhysicsEngine::Instance()->BodyInterface().AddImpulse(mBodyID,
        Math::Convert<Vec3>(inImpulse),
        Math::Convert<Vec3>(inPoint));
}

void Collider3D::AddAngularImpulse(Farg<glm::vec3> inAngularImpulse)
{
    ASSERT_BODYID()
    PhysicsEngine::Instance()->BodyInterface().AddAngularImpulse(mBodyID,
        Math::Convert<Vec3>(inAngularImpulse));
}

void Collider3D::SetLinearVelocity(Farg<glm::vec3> inLinearVelocity)
{
    ASSERT_BODYID()
    PhysicsEngine::Instance()->BodyInterface().SetLinearVelocity(mBodyID,
        Math::Convert<Vec3>(inLinearVelocity));
}

void Collider3D::OnContactAdded(ID inOtherColliderID,
    Farg<JPH::Body> inBody1,
    Farg<JPH::Body> inBody2,
    Farg<JPH::ContactManifold> manifold,
    JPH::ContactSettings& ioSettings)
{
    print_debug("Collider3D#{} '{}' contact added with Collider3D#{} '{}'",
            mUID(),
            mName,
            inOtherColliderID(),
            my_theatre()->GetThing(inOtherColliderID)->name());

    if(Console::try_GetVariable("Collider3D.debug_collision_msgs")->int_value)
    {
        print_debug("Collider3D#{} '{}' contact added with Collider3D#{} '{}'",
            mUID(),
            mName,
            inOtherColliderID(),
            my_theatre()->GetThing(inOtherColliderID)->name());
    }
}

void Collider3D::OnContactPersisted(ID inOtherColliderID,
    Farg<JPH::Body> inBody1,
    Farg<JPH::Body> inBody2,
    Farg<JPH::ContactManifold> manifold,
    JPH::ContactSettings& ioSettings)
{
    if(Console::try_GetVariable("Collider3D.debug_collision_persisted_msgs")->int_value)
        { print_debug("Collider3D#{} '{}' Contact Persisted", mUID(), mName); }
}

/*void Collider3D::OnContactRemoved(Farg<JPH::SubShapeIDPair> inSubShapeIDPair)
{
    print_debug("Collider3D#{} '{}' Contact Removed", mUID(), mName);
}*/

void Collider3D::OnBodyActivated()
{
    if(Console::try_GetVariable("Collider3D.debug_collision_msgs")->int_value)
        { print_debug("Collider3D#{} '{}' activated", mUID(), mName); }
}

void Collider3D::OnBodyDeactivated()
{
    if(Console::try_GetVariable("Collider3D.debug_collision_msgs")->int_value)
        { print_debug("Collider3D#{} '{}' deactivated", mUID(), mName); }
}
