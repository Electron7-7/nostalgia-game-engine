#include "transform_3d.hpp"
#include "world.hpp"

const glm::vec3& Transform3D::cOrigin() const
{ return m_Origin; }

const glm::vec3& Transform3D::cScale() const
{ return m_Scale; }

glm::vec3& Transform3D::Origin()
{ return m_Origin; }

glm::quat Transform3D::Quaternion() const
{ return m_Quaternion; }

glm::vec3 Transform3D::Euler(bool degrees) const
{ return (degrees) ? glm::degrees(m_Euler) : m_Euler; }

glm::vec3& Transform3D::Scale()
{ return m_Scale; }

void Transform3D::Quaternion(const glm::quat& quaternion)
{
    m_Quaternion = quaternion;
    m_Euler = glm::eulerAngles(quaternion);
}

void Transform3D::Euler(const glm::vec3& euler, bool degrees)
{
    m_Euler = (degrees) ? glm::radians(euler) : euler;
    m_Quaternion = glm::quat(m_Euler);
}

glm::vec3 Transform3D::Front() const
{ return (Quaternion() * World::Front()); }

glm::vec3 Transform3D::Right() const
{ return (Quaternion() * World::Right()); }

glm::vec3 Transform3D::Up() const
{ return (Quaternion() * World::Up()); }
