#include "actor_2d.hpp"
#include "theatre/theatre.hpp"

using namespace TheatreFile;

void Actor2D::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);

    data.get_variable(mLocalTransform.position, "Position", "Origin");
    data.get_variable(mLocalTransform.scale, "Scale", "Size", "OuuughImSoBigAndRound");
    if(data.get_variable(mLocalTransform.rotation_degrees, "RotationDegrees") == OK)
        { mLocalTransform.rotation_radians = glm::radians(mLocalTransform.rotation_degrees); }
    if(data.get_variable(mLocalTransform.rotation_radians, "Rotation", "RotationRadians") == OK)
        { mLocalTransform.rotation_degrees = glm::degrees(mLocalTransform.rotation_radians); }
    data.get_variable(mVisible, "Visible");

    data.get_variable(mDebugHighlight, "DebugHighlight");
}

Shared<ThingData> Actor2D::GetVariables() const
{
    auto data{Super::GetVariables()};

    data->set_variable(mLocalTransform.position, "Position");
    data->set_variable(mLocalTransform.scale, "Scale");
    data->set_variable(mLocalTransform.rotation_radians, "RotationRadians");
    data->set_variable(mVisible, "Visible");

    data->set_variable(mDebugHighlight, "DebugHighlight");

    return data;
}

void Actor2D::Ready()
{
    Super::Ready();
    if(auto parent_id{Theatre::Current()->GetParent(mUID)};
        parent_id.invalid() or not Theatre::Current()->DerivedFrom(parent_id, ThingType::Actor2D))
    {
        mGlobalTransform = mLocalTransform;
        _update_children_global_transform(true);
    }
}

Farg<glm::vec2> Actor2D::Position() const
{ return mLocalTransform.position; }

float Actor2D::Rotation() const
{ return mLocalTransform.rotation_radians; }

float Actor2D::RotationDegrees() const
{ return mLocalTransform.rotation_degrees; }

Farg<glm::vec2> Actor2D::Scale() const
{ return mLocalTransform.scale; }

glm::vec2 Actor2D::GlobalPosition() const
{ return mGlobalTransform.position; }

float Actor2D::GlobalRotation() const
{ return mGlobalTransform.rotation_radians; }

float Actor2D::GlobalRotationDegrees() const
{ return mGlobalTransform.rotation_degrees; }

glm::vec2 Actor2D::GlobalScale() const
{ return mGlobalTransform.scale; }

void Actor2D::SetPosition(Farg<glm::vec2> inPosition)
{
    mLocalTransform.position = inPosition;
    _update_global_transform(mParentGlobalTransform);
}

void Actor2D::SetRotation(float inRotation)
{
    mLocalTransform.rotation_radians = inRotation;
    mLocalTransform.rotation_degrees = glm::degrees(mLocalTransform.rotation_radians);
    _update_global_transform(mParentGlobalTransform);
}

void Actor2D::SetRotationDegrees(float inRotation)
{
    mLocalTransform.rotation_degrees = inRotation;
    mLocalTransform.rotation_radians = glm::radians(mLocalTransform.rotation_degrees);
    _update_global_transform(mParentGlobalTransform);
}

void Actor2D::SetScale(Farg<glm::vec2> inScale)
{
    mLocalTransform.scale = inScale;
    _update_global_transform(mParentGlobalTransform);
}

void Actor2D::_update_global_transform()
{ _update_global_transform(mParentGlobalTransform); }

void Actor2D::_update_global_transform(Farg<Transform2D> inTransform)
{
    auto cosine{std::cosf(inTransform.rotation_radians)};
    auto sine{std::sinf(inTransform.rotation_radians)};
    auto scaled_position{inTransform.scale * mLocalTransform.position};
    mGlobalTransform.position = inTransform.position + glm::vec2{
         (scaled_position.x * cosine) + (scaled_position.y * sine),
        -(scaled_position.x * sine)   + (scaled_position.y * cosine)
    };
    mGlobalTransform.scale = inTransform.scale * mLocalTransform.scale;
    mGlobalTransform.rotation_degrees = inTransform.rotation_degrees + mLocalTransform.rotation_degrees;
    mGlobalTransform.rotation_radians = inTransform.rotation_radians + mLocalTransform.rotation_radians;
    mParentGlobalTransform = inTransform;
    _update_children_global_transform();
}

void Actor2D::_update_children_global_transform(bool isCalledFromReady)
{
    if(not Theatre::Current()->IsStarted() and not isCalledFromReady)
        { return; }
    auto children{Theatre::Current()->GetChildren(mUID)};
    for(auto child_id : children)
    {
        if(not child_id.invalid() and Theatre::Current()->DerivedFrom(child_id, ThingType::Actor2D))
            { Theatre::Current()->GetThinker<Actor2D>(child_id)->_update_global_transform(mGlobalTransform); }
    }
}

void Actor2D::OnChildAdded(Relative inChild)
{
    if(Theatre::Current()->DerivedFrom(inChild.uid(), ThingType::Actor2D))
        { Theatre::Current()->GetThinker<Actor2D>(inChild.uid())->_update_global_transform(mGlobalTransform); }
}
