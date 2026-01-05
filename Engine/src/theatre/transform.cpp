#include "transform.hpp"
#include "settings/world.hpp"
#include "theatre/parser/thing_data.hpp"

void Transform2D::OnTransformChanged(PropertyChanged) {}

void Transform2D::SetTransformVariables(Farg<ThingData> data)
{
    data.GetVariable(mOrigin, "Origin", "Position");
    data.GetVariable(mScale, "Scale", "Size", "OuuughImSoBigAndRound");
    data.GetVariable(mRotation, "Rotation");
}

void Transform2D::GetTransformVariables(Shared<ThingData> data) const
{
    data->AddVariable(mOrigin, "Origin");
    data->AddVariable(mScale, "Scale");
    data->AddVariable(mRotation, "Rotation");
}

Farg<glm::vec2> Transform2D::Origin() const
{ return mOrigin; }

Farg<glm::vec2> Transform2D::Scale() const
{ return mScale; }

double Transform2D::Rotation(bool isDegrees) const
{ return (isDegrees) ? glm::degrees(mRotation) : mRotation; }

void Transform2D::Origin(Farg<glm::vec2> inOrigin)
{
    LockGuard<RMutex> lock{mTransformMutex};
    mOrigin = inOrigin;
    OnTransformChanged(ORIGIN);
}

void Transform2D::Scale(Farg<glm::vec2> inScale)
{
    LockGuard<RMutex> lock{mTransformMutex};
    mScale = inScale;
    OnTransformChanged(SCALE);
}

void Transform2D::Rotation(double inRotation, bool isDegrees)
{
    mRotation = (isDegrees)
        ? glm::radians(inRotation)
        : inRotation;
    OnTransformChanged(ROTATION);
}

void Transform3D::OnTransformChanged(PropertyChanged) {}

void Transform3D::SetTransformVariables(Farg<ThingData> data)
{
    data.GetVariable(mOrigin, "Origin", "Position");
    data.GetVariable(mScale, "Scale", "Size", "OuuughImSoBigAndRound");
    if(data.GetVariable(mQuaternion, "Quaternion"))
        { Quaternion(mQuaternion); }
    else
    {
        if(data.GetVariable(mEuler, "Rotation", "Euler"))
            { Quaternion({mEuler}); }
        else if(data.GetVariable(mEuler, "RotationDegrees", "EulerDegrees"))
            { Euler(mEuler, true); Quaternion({mEuler}); }
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
{
    return (degrees)
        ? glm::degrees(mEuler)
        : mEuler;
}

void Transform3D::Origin(Farg<glm::vec3> origin)
{
    LockGuard<RMutex> lock{mTransformMutex};
    mOrigin = origin;
    OnTransformChanged(ORIGIN);
}

void Transform3D::Scale(Farg<glm::vec3> scale)
{
    LockGuard<RMutex> lock{mTransformMutex};
    mScale = scale;
    OnTransformChanged(SCALE);
}

void Transform3D::Quaternion(Farg<glm::quat> quaternion)
{
    LockGuard<RMutex> lock{mTransformMutex};
    mQuaternion = quaternion;
    mEuler = glm::eulerAngles(mQuaternion);
    OnTransformChanged(ROTATION);
}

void Transform3D::Euler(Farg<glm::vec3> euler, bool degrees)
{
    LockGuard<RMutex> lock{mTransformMutex};
    mEuler = (degrees)
        ? glm::radians(euler)
        : euler;
    mQuaternion = glm::quat(mEuler);
    OnTransformChanged(ROTATION);
}

glm::vec3 Transform3D::Front() const
{ return (Quaternion() * Settings::World::Front()); }

glm::vec3 Transform3D::Right() const
{ return (Quaternion() * Settings::World::Right()); }

glm::vec3 Transform3D::Up() const
{ return (Quaternion() * Settings::World::Up()); }
