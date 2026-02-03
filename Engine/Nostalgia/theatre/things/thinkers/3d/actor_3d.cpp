#include "actor_3d.hpp"
#include <Nostalgia/theatre/theatre.hpp>

using namespace TheatreFile;

void Actor3D::SetVariables(Farg<ThingData> data)
{
    Thinker::SetVariables(data);

    data.get_variable(mPosition, "Position", "Origin");
    data.get_variable(mScale, "Scale", "Size", "OuuughImSoBigAndRound");
    if(data.get_variable(mEulerRotationDegrees, "RotationDegrees", "EulerDegrees") == OK)
        { SetRotationDegrees(mEulerRotationDegrees); }
    if(data.get_variable(mEulerRotationRadians, "Rotation", "Euler") == OK)
        { SetRotation(mEulerRotationRadians); }
    if(data.get_variable(mQuaternion, "Quaternion") == OK)
        { SetQuaternion(mQuaternion); }
}

Shared<ThingData> Actor3D::GetVariables() const
{
    auto data{Thinker::GetVariables()};

    data->set_variable(mPosition, "Position");
    data->set_variable(mQuaternion, "Quaternion");
    data->set_variable(mScale, "Scale");

    return data;
}

void Actor3D::SetPosition(Farg<glm::vec3> inPosition)
{ mPosition = inPosition; }

void Actor3D::SetQuaternion(Farg<glm::quat> inQuaternion)
{
    mQuaternion = glm::normalize(inQuaternion);
    mEulerRotationRadians = glm::eulerAngles(mQuaternion);
    mEulerRotationDegrees = glm::degrees(mEulerRotationRadians);
}

void Actor3D::SetRotation(Farg<glm::vec3> inRotation)
{
    mEulerRotationRadians = inRotation;
    mQuaternion = glm::normalize(glm::quat{mEulerRotationRadians});
    mEulerRotationDegrees = glm::degrees(mEulerRotationRadians);
}

void Actor3D::SetRotationDegrees(Farg<glm::vec3> inRotation)
{
    mEulerRotationDegrees = inRotation;
    mEulerRotationRadians = glm::radians(mEulerRotationDegrees);
    mQuaternion = glm::normalize(glm::quat{mEulerRotationRadians});
}

void Actor3D::SetScale(Farg<glm::vec3> inScale)
{ mScale = inScale; }

Farg<glm::vec3> Actor3D::Position() const
{ return mPosition; }

Farg<glm::quat> Actor3D::Quaternion() const
{ return mQuaternion; }

Farg<glm::vec3> Actor3D::Rotation() const
{ return mEulerRotationRadians; }

Farg<glm::vec3> Actor3D::RotationDegrees() const
{ return mEulerRotationDegrees; }

Farg<glm::vec3> Actor3D::Scale() const
{ return mScale; }

glm::vec3 Actor3D::GlobalPosition() const
{
    if(auto parent{my_theatre()->GetParent(mUID)};
        !parent.invalid() and my_theatre()->DerivedFrom(parent, ThingType::Actor3D))
            { return Position() + my_theatre()->GetThinker<Actor3D>(parent)->GlobalPosition(); }
    return Position();
}

glm::vec3 Actor3D::GlobalRotation() const
{
    if(auto parent{my_theatre()->GetParent(mUID)};
        !parent.invalid() and my_theatre()->DerivedFrom(parent, ThingType::Actor3D))
            { return Rotation() + my_theatre()->GetThinker<Actor3D>(parent)->GlobalRotation(); }
    return Rotation();
}

glm::vec3 Actor3D::GlobalRotationDegrees() const
{
    if(auto parent{my_theatre()->GetParent(mUID)};
        !parent.invalid() and my_theatre()->DerivedFrom(parent, ThingType::Actor3D))
            { return RotationDegrees() + my_theatre()->GetThinker<Actor3D>(parent)->GlobalRotationDegrees(); }
    return RotationDegrees();
}

glm::vec3 Actor3D::GlobalScale() const
{
    if(auto parent{my_theatre()->GetParent(mUID)};
        !parent.invalid() and my_theatre()->DerivedFrom(parent, ThingType::Actor3D))
            { return Scale() * my_theatre()->GetThinker<Actor3D>(parent)->GlobalScale(); }
    return Scale();
}

void Actor3D::SetGlobalPosition(Farg<glm::vec3> inPosition)
{
    if(auto parent{my_theatre()->GetParent(mUID)};
        !parent.invalid() and my_theatre()->DerivedFrom(parent, ThingType::Actor3D))
            { SetPosition(inPosition - my_theatre()->GetThinker<Actor3D>(parent)->GlobalPosition()); }
    SetPosition(inPosition);
}

void Actor3D::SetGlobalRotation(Farg<glm::vec3> inRotation)
{
    if(auto parent{my_theatre()->GetParent(mUID)};
        !parent.invalid() and my_theatre()->DerivedFrom(parent, ThingType::Actor3D))
            { SetRotation(inRotation - my_theatre()->GetThinker<Actor3D>(parent)->GlobalRotation()); }
    SetRotation(inRotation);
}

void Actor3D::SetGlobalRotationDegrees(Farg<glm::vec3> inRotationDegrees)
{
    if(auto parent{my_theatre()->GetParent(mUID)};
        !parent.invalid() and my_theatre()->DerivedFrom(parent, ThingType::Actor3D))
            { SetRotationDegrees(inRotationDegrees - my_theatre()->GetThinker<Actor3D>(parent)->GlobalRotationDegrees()); }
    SetRotationDegrees(inRotationDegrees);
}

void Actor3D::SetGlobalScale(Farg<glm::vec3> inScale)
{
    if(auto parent{my_theatre()->GetParent(mUID)};
        !parent.invalid() and my_theatre()->DerivedFrom(parent, ThingType::Actor3D))
            { SetScale(inScale - my_theatre()->GetThinker<Actor3D>(parent)->GlobalScale()); }
    SetScale(inScale);
}
