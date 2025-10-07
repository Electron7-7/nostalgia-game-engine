#include "transform_3d.hpp"
#include "world.hpp"

const glm::vec3& Transform3D::cOrigin() const
{ return mOrigin; }

const glm::vec3& Transform3D::cScale() const
{ return mScale; }

glm::vec3& Transform3D::Origin()
{ return mOrigin; }

glm::quat Transform3D::Quaternion() const
{ return mQuaternion; }

glm::vec3 Transform3D::Euler(bool degrees) const
{ return (degrees) ? glm::degrees(mEuler) : mEuler; }

glm::vec3& Transform3D::Scale()
{ return mScale; }

void Transform3D::Quaternion(const glm::quat& quaternion)
{
    mQuaternion = quaternion;
    mEuler = glm::eulerAngles(quaternion);
}

void Transform3D::Euler(const glm::vec3& euler, bool degrees)
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
