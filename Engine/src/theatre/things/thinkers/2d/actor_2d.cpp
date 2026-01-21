#include "actor_2d.hpp"
#include "theatre/parser.hpp"
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
    auto parent{m_pRootTheatre->GetParent(mUID)};
    if(parent.invalid()
        or !m_pRootTheatre->DerivedFrom(parent, ThingType::Actor2D))
            { return Position(); }
    return Position() + m_pRootTheatre->GetThinker<Actor2D>(parent)->GlobalPosition();
}

float Actor2D::GlobalRotation() const
{
    auto parent{m_pRootTheatre->GetParent(mUID)};
    if(parent.invalid()
        or !m_pRootTheatre->DerivedFrom(parent, ThingType::Actor2D))
            { return Rotation(); }
    return Rotation() + m_pRootTheatre->GetThinker<Actor2D>(parent)->GlobalRotation();
}

float Actor2D::GlobalRotationDegrees() const
{
    auto parent{m_pRootTheatre->GetParent(mUID)};
    if(parent.invalid()
        or !m_pRootTheatre->DerivedFrom(parent, ThingType::Actor2D))
            { return RotationDegrees(); }
    return RotationDegrees() + m_pRootTheatre->GetThinker<Actor2D>(parent)->GlobalRotationDegrees();
}

glm::vec2 Actor2D::GlobalScale() const
{
    auto parent{m_pRootTheatre->GetParent(mUID)};
    if(parent.invalid()
        or !m_pRootTheatre->DerivedFrom(parent, ThingType::Actor2D))
            { return Scale(); }
    return Scale() * m_pRootTheatre->GetThinker<Actor2D>(parent)->GlobalScale();
}

void Actor2D::SetGlobalPosition(Farg<glm::vec2> inPosition)
{
    auto parent{m_pRootTheatre->GetParent(mUID)};
    if(parent.invalid()
        or !m_pRootTheatre->DerivedFrom(parent, ThingType::Actor2D))
            { SetPosition(inPosition); }
    SetPosition(inPosition - m_pRootTheatre->GetThinker<Actor2D>(parent)->GlobalPosition());
}

void Actor2D::SetGlobalRotation(float inRotation)
{
    auto parent{m_pRootTheatre->GetParent(mUID)};
    if(parent.invalid()
        or !m_pRootTheatre->DerivedFrom(parent, ThingType::Actor2D))
            { SetRotation(inRotation); }
    SetRotation(inRotation - m_pRootTheatre->GetThinker<Actor2D>(parent)->GlobalRotation());
}

void Actor2D::SetGlobalRotationDegrees(float inRotationDegrees)
{
    auto parent{m_pRootTheatre->GetParent(mUID)};
    if(parent.invalid()
        or !m_pRootTheatre->DerivedFrom(parent, ThingType::Actor2D))
            { SetRotationDegrees(inRotationDegrees); }
    SetRotationDegrees(inRotationDegrees - m_pRootTheatre->GetThinker<Actor2D>(parent)->GlobalRotationDegrees());
}

void Actor2D::SetGlobalScale(Farg<glm::vec2> inScale)
{
    auto parent{m_pRootTheatre->GetParent(mUID)};
    if(parent.invalid()
        or !m_pRootTheatre->DerivedFrom(parent, ThingType::Actor2D))
            { SetScale(inScale); }
    SetScale(inScale - m_pRootTheatre->GetThinker<Actor2D>(parent)->GlobalScale());
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
