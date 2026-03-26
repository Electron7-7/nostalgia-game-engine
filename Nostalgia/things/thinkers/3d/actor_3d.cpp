#include "./actor_3d.hpp"
#include "theatre/theatre.hpp"

using namespace TheatreFile;

void Actor3D::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);

    data.get_variable(mLocalTransform.position, "Position", "Origin");
    data.get_variable(mLocalTransform.scale, "Scale", "Size", "OuuughImSoBigAndRound");
    if(data.get_variable(mLocalTransform.euler_rotation_degrees, "RotationDegrees", "EulerDegrees") == OK)
    {
        mLocalTransform.euler_rotation_radians = glm::radians(mLocalTransform.euler_rotation_degrees);
        mLocalTransform.quaternion = glm::normalize(glm::quat{mLocalTransform.euler_rotation_radians});
    }
    if(data.get_variable(mLocalTransform.euler_rotation_radians,
        "Rotation", "RotationRadians", "Euler", "EulerRadians") == OK)
    {
        mLocalTransform.quaternion = glm::normalize(glm::quat{mLocalTransform.euler_rotation_radians});
        mLocalTransform.euler_rotation_degrees = glm::degrees(mLocalTransform.euler_rotation_radians);
    }
    if(data.get_variable(mLocalTransform.quaternion, "Quaternion") == OK)
    {
        mLocalTransform.euler_rotation_radians = glm::radians(mLocalTransform.euler_rotation_degrees);
        mLocalTransform.quaternion = glm::normalize(glm::quat{mLocalTransform.euler_rotation_radians});
    }
}

Shared<ThingData> Actor3D::GetVariables() const
{
    auto data{Super::GetVariables()};

    data->set_variable(mLocalTransform.position, "Position");
    data->set_variable(mLocalTransform.quaternion, "Quaternion");
    data->set_variable(mLocalTransform.scale, "Scale");

    return data;
}

void Actor3D::Ready()
{
    Super::Ready();
    if(auto parent_id{Theatre::Current()->GetParent(uid())};
        parent_id.invalid() or not Theatre::Current()->DerivedFrom(parent_id, ThingType::Actor3D))
    {
        mGlobalTransform = mLocalTransform;
        _update_children_global_transform(true);
    }
}

Farg<glm::vec3> Actor3D::Position() const
{ return mLocalTransform.position; }

Farg<glm::quat> Actor3D::Quaternion() const
{ return mLocalTransform.quaternion; }

Farg<glm::vec3> Actor3D::Rotation() const
{ return mLocalTransform.euler_rotation_radians; }

Farg<glm::vec3> Actor3D::RotationDegrees() const
{ return mLocalTransform.euler_rotation_degrees; }

Farg<glm::vec3> Actor3D::Scale() const
{ return mLocalTransform.scale; }

glm::vec3 Actor3D::GlobalPosition() const
{ return mGlobalTransform.position; }

glm::vec3 Actor3D::GlobalRotation() const
{ return mGlobalTransform.euler_rotation_radians; }

glm::vec3 Actor3D::GlobalRotationDegrees() const
{ return mGlobalTransform.euler_rotation_degrees; }

glm::vec3 Actor3D::GlobalScale() const
{ return mGlobalTransform.scale; }

void Actor3D::SetPosition(Farg<glm::vec3> inPosition)
{
    mLocalTransform.position = inPosition;
    _update_global_transform(mParentGlobalTransform);
}

void Actor3D::SetQuaternion(Farg<glm::quat> inQuaternion)
{
    mLocalTransform.quaternion = glm::normalize(inQuaternion);
    mLocalTransform.euler_rotation_radians = glm::eulerAngles(mLocalTransform.quaternion);
    mLocalTransform.euler_rotation_degrees = glm::degrees(mLocalTransform.euler_rotation_radians);
    _update_global_transform(mParentGlobalTransform);
}

void Actor3D::SetRotation(Farg<glm::vec3> inRotation)
{
    mLocalTransform.euler_rotation_radians = inRotation;
    mLocalTransform.quaternion = glm::normalize(glm::quat{mLocalTransform.euler_rotation_radians});
    mLocalTransform.euler_rotation_degrees = glm::degrees(mLocalTransform.euler_rotation_radians);
    _update_global_transform(mParentGlobalTransform);
}

void Actor3D::SetRotationDegrees(Farg<glm::vec3> inRotation)
{
    mLocalTransform.euler_rotation_degrees = inRotation;
    mLocalTransform.euler_rotation_radians = glm::radians(mLocalTransform.euler_rotation_degrees);
    mLocalTransform.quaternion = glm::normalize(glm::quat{mLocalTransform.euler_rotation_radians});
    _update_global_transform(mParentGlobalTransform);
}

void Actor3D::SetScale(Farg<glm::vec3> inScale)
{
    mLocalTransform.scale = inScale;
    _update_global_transform(mParentGlobalTransform);
}

void Actor3D::_update_global_transform()
{ _update_global_transform(mParentGlobalTransform); }

void Actor3D::_update_global_transform(Farg<Transform3D> inTransform)
{
    mGlobalTransform.scale = inTransform.scale * mLocalTransform.scale;
    mGlobalTransform.euler_rotation_degrees = inTransform.euler_rotation_degrees + mLocalTransform.euler_rotation_degrees;
    mGlobalTransform.euler_rotation_radians = inTransform.euler_rotation_radians + mLocalTransform.euler_rotation_radians;
    mGlobalTransform.quaternion = inTransform.quaternion * mLocalTransform.quaternion;
    mGlobalTransform.position = mLocalTransform.position *
        glm::inverse(inTransform.quaternion) + inTransform.position;
    mParentGlobalTransform = inTransform;

    _update_children_global_transform();
}

void Actor3D::_update_children_global_transform(bool isCalledFromReady)
{
    if(not Theatre::Current()->IsStarted() and not isCalledFromReady)
        { return; }
    auto children{Theatre::Current()->GetChildren(uid())};
    for(auto child_id : children)
    {
        if(not child_id.invalid() and Theatre::Current()->DerivedFrom(child_id, ThingType::Actor3D))
            { Theatre::Current()->GetThinker<Actor3D>(child_id)->_update_global_transform(mGlobalTransform); }
    }
}

void Actor3D::OnChildAdded(Relative inChild)
{
    if(Theatre::Current()->DerivedFrom(inChild.uid(), ThingType::Actor3D))
        { Theatre::Current()->GetThinker<Actor3D>(inChild.uid())->_update_global_transform(mGlobalTransform); }
}
