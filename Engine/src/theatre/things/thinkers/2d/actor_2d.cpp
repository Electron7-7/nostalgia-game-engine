#include "actor_2d.hpp"
#include "theatre/parser/thing_data.hpp"

void Actor2D::SetVariables(Farg<ThingData> data)
{
    Thinker::SetVariables(data);

    data.GetVariable(mPosition, "Position", "Origin");
    data.GetVariable(mScale, "Scale", "Size", "OuuughImSoBigAndRound");
    if(data.GetVariable(mRotationDegrees, "RotationDegrees"))
        { SetRotationDegrees(mRotationDegrees); }
    if(data.GetVariable(mRotationRadians, "Rotation", "RotationRadians"))
        { SetRotation(mRotationRadians); }
    data.GetVariable(mVisible, "Visible");

    data.GetVariable(mDebugHighlight, "DebugHighlight");
    data.GetVariable(mDebugMeshInstanceID, "DebugMeshInstance");
}

Shared<ThingData> Actor2D::GetVariables() const
{
    auto data{Thinker::GetVariables()};

    data->AddVariable(mPosition, "Position");
    data->AddVariable(mScale, "Scale");
    data->AddVariable(mRotationRadians, "RotationRadians");
    data->AddVariable(mVisible, "Visible");

    data->AddVariable(mDebugHighlight, "DebugHighlight");
    data->AddVariable(mDebugMeshInstanceID, "DebugMeshInstance");

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

bool Actor2D::Visible() const
{ return mVisible; }

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

void Actor2D::SetVisible(bool isVisible)
{ mVisible = isVisible; }
