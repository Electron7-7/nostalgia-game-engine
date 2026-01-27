#include "actor_2d.hpp"
#include "theatre/theatre.hpp"

using namespace TheatreFile;

void Actor2D::SetVariables(Farg<ThingData> data)
{
    Thinker::SetVariables(data);

    data.get_variable(mPosition, "Position", "Origin");
    data.get_variable(mScale, "Scale", "Size", "OuuughImSoBigAndRound");
    if(data.get_variable(mRotationDegrees, "RotationDegrees") == OK)
        { SetRotationDegrees(mRotationDegrees); }
    if(data.get_variable(mRotationRadians, "Rotation", "RotationRadians") == OK)
        { SetRotation(mRotationRadians); }
    data.get_variable(mVisible, "Visible");

    data.get_variable(mDebugHighlight, "DebugHighlight");
}

Shared<ThingData> Actor2D::GetVariables() const
{
    auto data{Thinker::GetVariables()};

    data->set_variable(mPosition, "Position");
    data->set_variable(mScale, "Scale");
    data->set_variable(mRotationRadians, "RotationRadians");
    data->set_variable(mVisible, "Visible");

    data->set_variable(mDebugHighlight, "DebugHighlight");

    return data;
}

void Actor2D::Ready()
{ Thinker::Ready(); }

Farg<glm::vec2> Actor2D::Position() const
{ return mPosition; }

float Actor2D::Rotation() const
{ return mRotationRadians; }

float Actor2D::RotationDegrees() const
{ return mRotationDegrees; }

Farg<glm::vec2> Actor2D::Scale() const
{ return mScale; }

glm::vec2 Actor2D::GlobalPosition() const
{
    if(auto parent{my_theatre()->GetParent(mUID)};
        !parent.invalid() and !my_theatre()->DerivedFrom(parent, ThingType::Actor2D))
            { return Position() + my_theatre()->GetThinker<Actor2D>(parent)->GlobalPosition(); }
    return Position();
}

float Actor2D::GlobalRotation() const
{
    if(auto parent{my_theatre()->GetParent(mUID)};
        !parent.invalid() and my_theatre()->DerivedFrom(parent, ThingType::Actor2D))
            { return Rotation() + my_theatre()->GetThinker<Actor2D>(parent)->GlobalRotation(); }
    return Rotation();
}

float Actor2D::GlobalRotationDegrees() const
{
    if(auto parent{my_theatre()->GetParent(mUID)};
        !parent.invalid() and my_theatre()->DerivedFrom(parent, ThingType::Actor2D))
            { return RotationDegrees() + my_theatre()->GetThinker<Actor2D>(parent)->GlobalRotationDegrees(); }
    return RotationDegrees();
}

glm::vec2 Actor2D::GlobalScale() const
{
    if(auto parent{my_theatre()->GetParent(mUID)};
        !parent.invalid() and my_theatre()->DerivedFrom(parent, ThingType::Actor2D))
            { return Scale() * my_theatre()->GetThinker<Actor2D>(parent)->GlobalScale(); }
    return Scale();
}

void Actor2D::SetGlobalPosition(Farg<glm::vec2> inPosition)
{
    if(auto parent{my_theatre()->GetParent(mUID)};
        !parent.invalid() and my_theatre()->DerivedFrom(parent, ThingType::Actor2D))
            { SetPosition(inPosition - my_theatre()->GetThinker<Actor2D>(parent)->GlobalPosition()); }
    SetPosition(inPosition);
}

void Actor2D::SetGlobalRotation(float inRotation)
{
    if(auto parent{my_theatre()->GetParent(mUID)};
        !parent.invalid() and my_theatre()->DerivedFrom(parent, ThingType::Actor2D))
            { SetRotation(inRotation - my_theatre()->GetThinker<Actor2D>(parent)->GlobalRotation()); }
    SetRotation(inRotation);
}

void Actor2D::SetGlobalRotationDegrees(float inRotationDegrees)
{
    if(auto parent{my_theatre()->GetParent(mUID)};
        !parent.invalid() and my_theatre()->DerivedFrom(parent, ThingType::Actor2D))
            { SetRotationDegrees(inRotationDegrees - my_theatre()->GetThinker<Actor2D>(parent)->GlobalRotationDegrees()); }
    SetRotationDegrees(inRotationDegrees);
}

void Actor2D::SetGlobalScale(Farg<glm::vec2> inScale)
{
    if(auto parent{my_theatre()->GetParent(mUID)};
        !parent.invalid() and my_theatre()->DerivedFrom(parent, ThingType::Actor2D))
            { SetScale(inScale - my_theatre()->GetThinker<Actor2D>(parent)->GlobalScale()); }
    SetScale(inScale);
}

void Actor2D::SetPosition(Farg<glm::vec2> inPosition)
{ mPosition = inPosition; }

void Actor2D::SetRotation(float inRotation)
{
    mRotationRadians = inRotation;
    mRotationDegrees = glm::degrees(mRotationRadians);
}

void Actor2D::SetRotationDegrees(float inRotation)
{
    mRotationDegrees = inRotation;
    mRotationRadians = glm::radians(mRotationDegrees);
}

void Actor2D::SetScale(Farg<glm::vec2> inScale)
{ mScale = inScale; }
