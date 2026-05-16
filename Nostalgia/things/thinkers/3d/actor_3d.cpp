#include "./actor_3d.hpp"
#include "../../thing_factory.hpp"
#include "theatre/theatre.hpp"

#define LOCK_TRANSFORM LockGuard<RMutex> trans_lock{mTransformMutex}

using namespace TheatreFile;

void Actor3D::InitVariables()
{
    Super::InitVariables();
}

void Actor3D::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);

    LOCK_TRANSFORM;

    data.get_variable(mLocalTrans.position, "Position", "Origin");
    data.get_variable(mLocalTrans.scale, "Scale", "Size", "OuuughImSoBigAndRound");
    if(data.get_variable(mLocalTrans.rotation_degrees, "RotationDegrees", "EulerDegrees") == OK)
    {
        mLocalTrans.rotation = glm::radians(mLocalTrans.rotation_degrees);
        mLocalTrans.quaternion = glm::normalize(glm::quat{mLocalTrans.rotation});
    }
    else if(data.get_variable(mLocalTrans.rotation,
        "Rotation", "RotationRadians", "Euler", "EulerRadians") == OK)
    {
        mLocalTrans.quaternion = glm::normalize(glm::quat{mLocalTrans.rotation});
        mLocalTrans.rotation_degrees = glm::degrees(mLocalTrans.rotation);
    }
    else if(data.get_variable(mLocalTrans.quaternion, "Quaternion") == OK)
    {
        mLocalTrans.rotation = glm::radians(mLocalTrans.rotation_degrees);
        mLocalTrans.quaternion = glm::normalize(glm::quat{mLocalTrans.rotation});
    }
    _update_global_transform();
}

Shared<ThingData> Actor3D::GetVariables() const
{
    auto data{Super::GetVariables()};

    LOCK_TRANSFORM;

    data->set_variable(mLocalTrans.position, "Position");
    data->set_variable(mLocalTrans.rotation_degrees, "RotationDegrees");
    data->set_variable(mLocalTrans.rotation, "Rotation");
    data->set_variable(mLocalTrans.quaternion, "Quaternion");
    data->set_variable(mLocalTrans.scale, "Scale");
    data->hide_from_editor("Quaternion");

    return data;
}

void Actor3D::Ready()
{
    Super::Ready();
    // Only update top-level Actors, since this will recurse through their children
    if(auto parent_id{Theatre::Current()->GetParent(uid())};
        parent_id.invalid() or not ThingFactory::DerivedFrom(parent_id, ThingType::Actor3D))
    {
        mGlobalTrans = mLocalTrans;
        _update_children_global_transform();
    }
}

Farg<glm::vec3> Actor3D::Position() const
{
    LOCK_TRANSFORM;
    return mLocalTrans.position;
}

Farg<glm::quat> Actor3D::Quaternion() const
{
    LOCK_TRANSFORM;
    return mLocalTrans.quaternion;
}

Farg<glm::vec3> Actor3D::Rotation() const
{
    LOCK_TRANSFORM;
    return mLocalTrans.rotation;
}

Farg<glm::vec3> Actor3D::RotationDegrees() const
{
    LOCK_TRANSFORM;
    return mLocalTrans.rotation_degrees;
}

Farg<glm::vec3> Actor3D::Scale() const
{
    LOCK_TRANSFORM;
    return mLocalTrans.scale;
}

Farg<Transform3D> Actor3D::Transform() const
{ return mLocalTrans; }

Farg<glm::vec3> Actor3D::GlobalPosition() const
{
    LOCK_TRANSFORM;
    return mGlobalTrans.position;
}

Farg<glm::vec3> Actor3D::GlobalRotation() const
{
    LOCK_TRANSFORM;
    return mGlobalTrans.rotation;
}

Farg<glm::vec3> Actor3D::GlobalRotationDegrees() const
{
    LOCK_TRANSFORM;
    return mGlobalTrans.rotation_degrees;
}

Farg<glm::vec3> Actor3D::GlobalScale() const
{
    LOCK_TRANSFORM;
    return mGlobalTrans.scale;
}

Farg<Transform3D> Actor3D::GlobalTransform() const
{ return mGlobalTrans; }

void Actor3D::SetPosition(Farg<glm::vec3> inPosition)
{
    LOCK_TRANSFORM;
    mLocalTrans.position = inPosition;
    _update_global_transform();
}

void Actor3D::SetQuaternion(Farg<glm::quat> inQuaternion)
{
    LOCK_TRANSFORM;
    mLocalTrans.quaternion = glm::normalize(inQuaternion);
    mLocalTrans.rotation = glm::eulerAngles(mLocalTrans.quaternion);
    mLocalTrans.rotation_degrees = glm::degrees(mLocalTrans.rotation);
    _update_global_transform();
}

void Actor3D::SetRotation(Farg<glm::vec3> inRotation)
{
    LOCK_TRANSFORM;
    mLocalTrans.rotation = inRotation;
    mLocalTrans.quaternion = glm::normalize(glm::quat{mLocalTrans.rotation});
    mLocalTrans.rotation_degrees = glm::degrees(mLocalTrans.rotation);
    _update_global_transform();
}

void Actor3D::SetRotationDegrees(Farg<glm::vec3> inRotation)
{
    LOCK_TRANSFORM;
    mLocalTrans.rotation_degrees = inRotation;
    mLocalTrans.rotation = glm::radians(mLocalTrans.rotation_degrees);
    mLocalTrans.quaternion = glm::normalize(glm::quat{mLocalTrans.rotation});
    _update_global_transform();
}

void Actor3D::SetScale(Farg<glm::vec3> inScale)
{
    LOCK_TRANSFORM;
    mLocalTrans.scale = inScale;
    _update_global_transform();
}

void Actor3D::SetTransform(Farg<Transform3D> inTransform)
{
    LOCK_TRANSFORM;
    mLocalTrans = inTransform;
    _update_global_transform();
}

void Actor3D::SetGlobalPosition(Farg<glm::vec3> inPosition)
{
    LOCK_TRANSFORM;
    SetPosition(inPosition - mParentGlobalTrans.position);
}

void Actor3D::SetGlobalRotation(Farg<glm::vec3> inRotation)
{
    LOCK_TRANSFORM;
    SetRotation(inRotation - mParentGlobalTrans.rotation);
}

void Actor3D::SetGlobalRotationDegrees(Farg<glm::vec3> inRotationDegrees)
{
    LOCK_TRANSFORM;
    SetRotationDegrees(inRotationDegrees - mParentGlobalTrans.rotation_degrees);
}

void Actor3D::SetGlobalScale(Farg<glm::vec3> inScale)
{
    LOCK_TRANSFORM;
    SetScale(inScale / mParentGlobalTrans.scale);
}

void Actor3D::SetGlobalTransform(Farg<Transform3D> inTransform)
{
    LOCK_TRANSFORM;
    SetGlobalScale(inTransform.scale);
    SetGlobalRotation(inTransform.rotation);
    SetGlobalPosition(inTransform.position);
}

void Actor3D::OnChildAdded(Relative inChild)
{ _update_child_global_transform(inChild.uid); }

void Actor3D::OnParentChanged(Relative inNew, Relative inOld)
{
    LOCK_TRANSFORM;
    if(inNew.invalid() or not ThingFactory::DerivedFrom(inNew.uid, ThingType::Actor3D))
    {
        mLocalTrans = mGlobalTrans;
        mParentGlobalTrans = Transform3D{};
        _update_global_transform();
    }
}

void Actor3D::_set_parent_global_transform(Farg<Transform3D> inTransform)
{
    LOCK_TRANSFORM;
    mParentGlobalTrans = inTransform;
    _update_global_transform();
}

void Actor3D::_update_global_transform()
{
    LOCK_TRANSFORM;
    mGlobalTrans.scale = mParentGlobalTrans.scale * mLocalTrans.scale;
    mGlobalTrans.quaternion = mParentGlobalTrans.quaternion * mLocalTrans.quaternion;
    mGlobalTrans.rotation = mParentGlobalTrans.rotation + mLocalTrans.rotation;
    mGlobalTrans.rotation_degrees = glm::degrees(mGlobalTrans.rotation);
    mGlobalTrans.position = mParentGlobalTrans.model_matrix() * mLocalTrans.model_matrix()[3];
    _update_children_global_transform();
}

void Actor3D::_update_children_global_transform()
{
    auto children{Theatre::Current()->GetChildren(uid())};
    for(auto child_id : children)
        { _update_child_global_transform(child_id); }
}

void Actor3D::_update_child_global_transform(ID inUID)
{
    if(ThingFactory::DerivedFrom(inUID, ThingType::Actor3D))
        { ThingFactory::GetThing<Actor3D>(inUID)->_set_parent_global_transform(mGlobalTrans); }
}
