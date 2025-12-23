#include "transform_3d.hpp"
#include "settings/world.hpp"
#include "theatre/parser/thing_data.hpp"

void Transform3D::SetTransformVariables(Farg<ThingData> data)
{
    data.GetVariable(mOrigin, "Origin", "Position");
    data.GetVariable(mScale, "Scale", "Size", "OuuughImSoBigAndRound");
    if(!data.GetVariable(mQuaternion, "Quaternion"))
    {
        if(data.GetVariable(mEuler, "Rotation", "Euler"))
            { SetQuaternion({mEuler}); }
        else if(data.GetVariable(mEuler, "RotationDegrees", "EulerDegrees"))
            { SetEuler(mEuler, true); SetQuaternion({mEuler}); }
    }
}

void Transform3D::GetTransformVariables(Shared<ThingData> data) const
{
    data->AddVariable(mOrigin, "Origin");
    data->AddVariable(mScale, "Scale");
    data->AddVariable(mQuaternion, "Quaternion");
}

Farg<glm::vec3> Transform3D::Origin() const
{ return mOrigin; }

Farg<glm::vec3> Transform3D::Scale() const
{ return mScale; }

Farg<glm::quat> Transform3D::Quaternion() const
{ return mQuaternion; }

glm::vec3 Transform3D::Euler(bool degrees) const
{ return (degrees) ? glm::degrees(mEuler) : mEuler; }

void Transform3D::SetOrigin(Farg<glm::vec3> origin)
{ mOrigin = origin; }

void Transform3D::SetScale(Farg<glm::vec3> scale)
{ mScale = scale; }

void Transform3D::SetQuaternion(Farg<glm::quat> quaternion)
{
    mQuaternion = quaternion;
    mEuler = glm::eulerAngles(mQuaternion);
}

void Transform3D::SetEuler(Farg<glm::vec3> euler, bool degrees)
{
    mEuler = (degrees)
        ? glm::radians(euler)
        : euler;
    mQuaternion = glm::quat(mEuler);
}

glm::vec3 Transform3D::Front() const
{ return (Quaternion() * Settings::World::Front()); }

glm::vec3 Transform3D::Right() const
{ return (Quaternion() * Settings::World::Right()); }

glm::vec3 Transform3D::Up() const
{ return (Quaternion() * Settings::World::Up()); }
