#include "transform_3d.hpp"
#include "world.hpp"

const glm::vec3& Transform3D::Origin() const
{ return mOrigin; }

const glm::vec3& Transform3D::Scale() const
{ return mScale; }

const glm::quat& Transform3D::Quaternion() const
{ return mQuaternion; }

glm::vec3 Transform3D::Euler(bool degrees) const
{ return (degrees) ? glm::degrees(mEuler) : mEuler; }

void Transform3D::SetOrigin(const glm::vec3& origin)
{ mOrigin = origin; }

void Transform3D::SetScale(const glm::vec3& scale)
{ mScale = scale; }

void Transform3D::SetQuaternion(const glm::quat& quaternion)
{
    mQuaternion = quaternion;
    mEuler = glm::eulerAngles(mQuaternion);
}

void Transform3D::SetEuler(const glm::vec3& euler, bool degrees)
{
    mEuler = (degrees) ? glm::radians(euler) : euler;
    mQuaternion = glm::quat(mEuler);
}

glm::vec3 Transform3D::Front() const
{ return (Quaternion() * World::Front()); }

glm::vec3 Transform3D::Right() const
{ return (Quaternion() * World::Right()); }

glm::vec3 Transform3D::Up() const
{ return (Quaternion() * World::Up()); }
