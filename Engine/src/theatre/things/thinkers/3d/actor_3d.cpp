#include "actor_3d.hpp"
#include "collider_3d.hpp"
#include "managers/theatre_manager.hpp"
#include "theatre/parser.hpp"

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
    data.get_variable(mVisible, "Visible");

    data.get_variable(mDebugHighlight, "DebugHighlight");
    data.get_variable(mDebugMeshInstanceID, "DebugMeshInstance");
}

Shared<ThingData> Actor3D::GetVariables() const
{
    auto data{Thinker::GetVariables()};

    data->set_variable(mPosition, "Position");
    data->set_variable(mQuaternion, "Quaternion");
    data->set_variable(mScale, "Scale");
    data->set_variable(mVisible, "Visible");

    data->set_variable(mDebugHighlight, "DebugHighlight");
    data->set_variable(mDebugMeshInstanceID, "DebugMeshInstance");

    return data;
}

void Actor3D::Ready()
{ Thinker::Ready(); }

void Actor3D::Tick()
{
    for(auto child : mChildren)
    {
        if(auto collider{DCast<Collider3D>(g_pTheatreManager
                ->CurrentTheatre()
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
