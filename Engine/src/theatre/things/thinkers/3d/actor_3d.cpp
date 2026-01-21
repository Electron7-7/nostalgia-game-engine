#include "actor_3d.hpp"
#include "theatre/parser.hpp"
#include "theatre/theatre.hpp"

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
    auto parent{m_pRootTheatre->GetParent(mUID)};
    if(parent.invalid()
        or !m_pRootTheatre->DerivedFrom(parent, ThingType::Actor3D))
            { return Position(); }
    return Position() + m_pRootTheatre->GetThinker<Actor3D>(parent)->GlobalPosition();
}

glm::vec3 Actor3D::GlobalRotation() const
{
    auto parent{m_pRootTheatre->GetParent(mUID)};
    if(parent.invalid()
        or !m_pRootTheatre->DerivedFrom(parent, ThingType::Actor3D))
            { return Rotation(); }
    return Rotation() + m_pRootTheatre->GetThinker<Actor3D>(parent)->GlobalRotation();
}

glm::vec3 Actor3D::GlobalRotationDegrees() const
{
    auto parent{m_pRootTheatre->GetParent(mUID)};
    if(parent.invalid()
        or !m_pRootTheatre->DerivedFrom(parent, ThingType::Actor3D))
            { return RotationDegrees(); }
    return RotationDegrees() + m_pRootTheatre->GetThinker<Actor3D>(parent)->GlobalRotationDegrees();
}

glm::vec3 Actor3D::GlobalScale() const
{
    auto parent{m_pRootTheatre->GetParent(mUID)};
    if(parent.invalid()
        or !m_pRootTheatre->DerivedFrom(parent, ThingType::Actor3D))
            { return Scale(); }
    return Scale() * m_pRootTheatre->GetThinker<Actor3D>(parent)->GlobalScale();
}

void Actor3D::SetGlobalPosition(Farg<glm::vec3> inPosition)
{
    auto parent{m_pRootTheatre->GetParent(mUID)};
    if(parent.invalid()
        or !m_pRootTheatre->DerivedFrom(parent, ThingType::Actor3D))
            { SetPosition(inPosition); }
    SetPosition(inPosition - m_pRootTheatre->GetThinker<Actor3D>(parent)->GlobalPosition());
}

void Actor3D::SetGlobalRotation(Farg<glm::vec3> inRotation)
{
    auto parent{m_pRootTheatre->GetParent(mUID)};
    if(parent.invalid()
        or !m_pRootTheatre->DerivedFrom(parent, ThingType::Actor3D))
            { SetRotation(inRotation); }
    SetRotation(inRotation - m_pRootTheatre->GetThinker<Actor3D>(parent)->GlobalRotation());
}

void Actor3D::SetGlobalRotationDegrees(Farg<glm::vec3> inRotationDegrees)
{
    auto parent{m_pRootTheatre->GetParent(mUID)};
    if(parent.invalid()
        or !m_pRootTheatre->DerivedFrom(parent, ThingType::Actor3D))
            { SetRotationDegrees(inRotationDegrees); }
    SetRotationDegrees(inRotationDegrees - m_pRootTheatre->GetThinker<Actor3D>(parent)->GlobalRotationDegrees());
}

void Actor3D::SetGlobalScale(Farg<glm::vec3> inScale)
{
    auto parent{m_pRootTheatre->GetParent(mUID)};
    if(parent.invalid()
        or !m_pRootTheatre->DerivedFrom(parent, ThingType::Actor3D))
            { SetScale(inScale); }
    SetScale(inScale - m_pRootTheatre->GetThinker<Actor3D>(parent)->GlobalScale());
}
