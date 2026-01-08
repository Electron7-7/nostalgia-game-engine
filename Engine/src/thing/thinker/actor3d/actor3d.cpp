#include "actor3d.hpp"
#include "collider3d.hpp"
#include "managers/theatre_manager.hpp"
#include "theatre/parser/thing_data.hpp"

void Actor3D::SetVariables(Farg<ThingData> data)
{
    Thinker::SetVariables(data);

    data.GetVariable(mPosition, "Position", "Origin");
    data.GetVariable(mScale, "Scale", "Size", "OuuughImSoBigAndRound");
    if(data.GetVariable(mEulerRotationDegrees, "RotationDegrees", "EulerDegrees"))
        { RotationDegrees(mEulerRotationDegrees); }
    if(data.GetVariable(mEulerRotationRadians, "Rotation", "Euler"))
        { Rotation(mEulerRotationRadians); }
    if(data.GetVariable(mQuaternion, "Quaternion"))
        { Quaternion(mQuaternion); }
    data.GetVariable(mVisible, "Visible");

    data.GetVariable(mDebugHighlight, "DebugHighlight");
    data.GetVariable(mDebugMeshInstanceID, "DebugMeshInstance");
}

Shared<ThingData> Actor3D::GetVariables() const
{
    auto data{Thinker::GetVariables()};

    data->AddVariable(mPosition, "Position");
    data->AddVariable(mQuaternion, "Quaternion");
    data->AddVariable(mScale, "Scale");
    data->AddVariable(mVisible, "Visible");

    data->AddVariable(mDebugHighlight, "DebugHighlight");
    data->AddVariable(mDebugMeshInstanceID, "DebugMeshInstance");

    return data;
}

void Actor3D::Ready()
{ Thinker::Ready(); }

void Actor3D::Tick()
{
    for(auto child : mChildren)
    {
        if(auto collider{DCast<Collider>(g_pTheatreManager->GetThing<Collider>(child.id))};
            !collider->uid().invalid())
        {
            Position(collider->Position());
            Scale(collider->Scale());
            Quaternion(collider->Quaternion());
        }
    }
}

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

void Actor3D::Position(Farg<glm::vec3> inPosition)
{ mPosition = inPosition; }

void Actor3D::Quaternion(Farg<glm::quat> inQuaternion)
{
    mQuaternion = glm::normalize(inQuaternion);
    mEulerRotationRadians = glm::eulerAngles(mQuaternion);
    mEulerRotationDegrees = glm::degrees(mEulerRotationRadians);
}

void Actor3D::Rotation(Farg<glm::vec3> inRotation)
{
    mEulerRotationRadians = inRotation;
    mQuaternion = glm::normalize(glm::quat{mEulerRotationRadians});
    mEulerRotationDegrees = glm::degrees(mEulerRotationRadians);
}

void Actor3D::RotationDegrees(Farg<glm::vec3> inRotation)
{
    mEulerRotationDegrees = inRotation;
    mEulerRotationRadians = glm::radians(mEulerRotationDegrees);
    mQuaternion = glm::normalize(glm::quat{mEulerRotationRadians});
}

void Actor3D::Scale(Farg<glm::vec3> inScale)
{ mScale = inScale; }

bool Actor3D::Visible() const
{ return mVisible; }

void Actor3D::Visible(bool isVisible)
{ mVisible = isVisible; }
