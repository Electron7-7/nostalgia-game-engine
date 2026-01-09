#include "actor_3d.hpp"
#include "collider_3d.hpp"
#include "managers/theatre_manager.hpp"
#include "theatre/parser/thing_data.hpp"

void Actor3D::SetVariables(Farg<ThingData> data)
{
    Thinker::SetVariables(data);

    data.GetVariable(mPosition, "Position", "Origin");
    data.GetVariable(mScale, "Scale", "Size", "OuuughImSoBigAndRound");
    if(data.GetVariable(mEulerRotationDegrees, "RotationDegrees", "EulerDegrees"))
        { SetRotationDegrees(mEulerRotationDegrees); }
    if(data.GetVariable(mEulerRotationRadians, "Rotation", "Euler"))
        { SetRotation(mEulerRotationRadians); }
    if(data.GetVariable(mQuaternion, "Quaternion"))
        { SetQuaternion(mQuaternion); }
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
        if(auto collider{DCast<Collider3D>(g_pTheatreManager
                ->GetThinker<Collider3D>(child.id))};
            !collider->uid().invalid())
        {
            SetPosition(collider->Position());
            SetScale(collider->Scale());
            SetQuaternion(collider->Quaternion());
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

bool Actor3D::Visible() const
{ return mVisible; }

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

void Actor3D::SetVisible(bool isVisible)
{ mVisible = isVisible; }
