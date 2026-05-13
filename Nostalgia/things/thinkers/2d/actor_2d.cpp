#include "./actor_2d.hpp"
#include "../../thing_data.hpp"
#include "../../thing_factory.hpp"
#include "theatre/theatre.hpp"

#define LOCK_TRANSFORM LockGuard<RMutex> _lock{mTransformMutex};

using namespace TheatreFile;

void Actor2D::InitVariables()
{
    Super::InitVariables();
}

void Actor2D::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);

    LOCK_TRANSFORM

    data.get_variable(mLocalTransform.position, "Position", "Origin");
    data.get_variable(mLocalTransform.scale, "Scale", "Size", "OuuughImSoBigAndRound");
    if(data.get_variable(mLocalTransform.rotation_degrees, "RotationDegrees") == OK)
        { mLocalTransform.rotation_radians = glm::radians(mLocalTransform.rotation_degrees); }
    else if(data.get_variable(mLocalTransform.rotation_radians, "Rotation", "RotationRadians") == OK)
        { mLocalTransform.rotation_degrees = glm::degrees(mLocalTransform.rotation_radians); }
    _update_global_transform();
}

Shared<ThingData> Actor2D::GetVariables() const
{
    auto data{Super::GetVariables()};

    LOCK_TRANSFORM

    data->set_variable(mLocalTransform.position, "Position");
    data->set_variable(mLocalTransform.scale, "Scale");
    data->set_variable(mLocalTransform.rotation_degrees, "RotationDegrees");
    data->set_variable(mLocalTransform.rotation_radians, "Rotation");
    data->set_variable(mVisible, "Visible");

    return data;
}

void Actor2D::Ready()
{
    Super::Ready();
    // Only update top-level Actors, since this will recurse through their children
    if(auto parent_id{Theatre::Current()->GetParent(uid())};
        parent_id.invalid() or not ThingFactory::DerivedFrom(parent_id, ThingType::Actor2D))
    {
        mGlobalTransform = mLocalTransform;
        _update_children_global_transform();
    }
}

Farg<glm::vec2> Actor2D::Position() const
{
    LOCK_TRANSFORM
    return mLocalTransform.position;
}

float Actor2D::Rotation() const
{
    LOCK_TRANSFORM
    return mLocalTransform.rotation_radians;
}

float Actor2D::RotationDegrees() const
{
    LOCK_TRANSFORM
    return mLocalTransform.rotation_degrees;
}

Farg<glm::vec2> Actor2D::Scale() const
{
    LOCK_TRANSFORM
    return mLocalTransform.scale;
}

Farg<glm::vec2> Actor2D::GlobalPosition() const
{
    LOCK_TRANSFORM
    return mGlobalTransform.position;
}

float Actor2D::GlobalRotation() const
{
    LOCK_TRANSFORM
    return mGlobalTransform.rotation_radians;
}

float Actor2D::GlobalRotationDegrees() const
{
    LOCK_TRANSFORM
    return mGlobalTransform.rotation_degrees;
}

Farg<glm::vec2> Actor2D::GlobalScale() const
{
    LOCK_TRANSFORM
    return mGlobalTransform.scale;
}

Farg<Transform2D> Actor2D::GlobalTransform() const
{
    LOCK_TRANSFORM
    return mGlobalTransform;
}

void Actor2D::SetPosition(Farg<glm::vec2> inPosition)
{
    LOCK_TRANSFORM
    mLocalTransform.position = inPosition;
    _update_global_transform();
}

void Actor2D::SetRotation(float inRotation)
{
    LOCK_TRANSFORM
    mLocalTransform.rotation_radians = inRotation;
    mLocalTransform.rotation_degrees = glm::degrees(mLocalTransform.rotation_radians);
    _update_global_transform();
}

void Actor2D::SetRotationDegrees(float inRotation)
{
    LOCK_TRANSFORM
    mLocalTransform.rotation_degrees = inRotation;
    mLocalTransform.rotation_radians = glm::radians(mLocalTransform.rotation_degrees);
    _update_global_transform();
}

void Actor2D::SetScale(Farg<glm::vec2> inScale)
{
    LOCK_TRANSFORM
    mLocalTransform.scale = inScale;
    _update_global_transform();
}

void Actor2D::SetTransform(Farg<Transform2D> inTransform)
{
    LOCK_TRANSFORM
    mLocalTransform = inTransform;
    _update_global_transform();
}

void Actor2D::SetGlobalPosition(Farg<glm::vec2> inPosition)
{
    LOCK_TRANSFORM;
    SetPosition(inPosition - mParentGlobalTransform.position);
}

void Actor2D::SetGlobalRotation(float inRotationRadians)
{
    LOCK_TRANSFORM;
    SetRotation(inRotationRadians - mParentGlobalTransform.rotation_radians);
}

void Actor2D::SetGlobalRotationDegrees(float inRotationDegrees)
{
    LOCK_TRANSFORM;
    SetRotationDegrees(inRotationDegrees - mParentGlobalTransform.rotation_degrees);
}

void Actor2D::SetGlobalScale(Farg<glm::vec2> inScale)
{
    LOCK_TRANSFORM;
    SetScale(inScale / mParentGlobalTransform.scale);
}

void Actor2D::SetGlobalTransform(Farg<Transform2D> inTransform)
{
    LOCK_TRANSFORM;
    SetGlobalScale(inTransform.scale);
    SetGlobalRotation(inTransform.rotation_radians);
    SetGlobalPosition(inTransform.position);
}

void Actor2D::OnChildAdded(Relative inChild)
{ _update_child_global_transform(inChild.uid); }

void Actor2D::OnParentChanged(Relative inNew, Relative inOld)
{
    LOCK_TRANSFORM;
    if(inNew.invalid() or not ThingFactory::DerivedFrom(inNew.uid, ThingType::Actor2D))
    {
        mLocalTransform = mGlobalTransform;
        mParentGlobalTransform = Transform2D{};
    }
    _update_global_transform();
}

void Actor2D::_set_parent_global_transform(Farg<Transform2D> inTransform)
{
    LOCK_TRANSFORM;
    mParentGlobalTransform = inTransform;
    _update_global_transform();
}

void Actor2D::_update_global_transform()
{
    LOCK_TRANSFORM;
    auto cosine{std::cosf(mParentGlobalTransform.rotation_radians)};
    auto sine{std::sinf(mParentGlobalTransform.rotation_radians)};
    auto scaled_position{mParentGlobalTransform.scale * mLocalTransform.position};
    mGlobalTransform.position = mParentGlobalTransform.position + glm::vec2{
         (scaled_position.x * cosine) + (scaled_position.y * sine),
        -(scaled_position.x * sine)   + (scaled_position.y * cosine)
    };
    mGlobalTransform.scale = mParentGlobalTransform.scale * mLocalTransform.scale;
    mGlobalTransform.rotation_degrees = mParentGlobalTransform.rotation_degrees + mLocalTransform.rotation_degrees;
    mGlobalTransform.rotation_radians = mParentGlobalTransform.rotation_radians + mLocalTransform.rotation_radians;
    _update_children_global_transform();
}

void Actor2D::_update_children_global_transform()
{
    auto children{Theatre::Current()->GetChildren(uid())};
    for(auto child_id : children)
        { _update_child_global_transform(child_id); }
}

void Actor2D::_update_child_global_transform(ID inUID)
{
    if(ThingFactory::DerivedFrom(inUID, ThingType::Actor2D))
        { ThingFactory::GetThing<Actor2D>(inUID)->_set_parent_global_transform(mGlobalTransform); }
}
