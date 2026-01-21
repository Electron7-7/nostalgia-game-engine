#include "actor_2d.hpp"
#include "theatre/parser.hpp"

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
