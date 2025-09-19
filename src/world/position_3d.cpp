#include "position_3d.hpp"
#include "world.hpp"

void Position3D::SetOrigin(const glm::vec3& Origin)
{ m_Origin = Origin; }

void Position3D::SetRotation(const glm::vec3& Rotation, bool Degrees)
{ m_Rotation = (Degrees) ? glm::radians(Rotation) : Rotation; }

void Position3D::SetQuaternion(const glm::quat& Quaternion)
{ m_Rotation = glm::eulerAngles(Quaternion); }

void Position3D::SetScale(const glm::vec3& Scale)
{ m_Scale = Scale; }

glm::vec3 Position3D::Origin() const
{ return m_Origin; }

glm::vec3 Position3D::Rotation(bool Degrees) const
{ return (Degrees) ? glm::degrees(m_Rotation) : m_Rotation; }

glm::quat Position3D::Quaternion() const
{ return glm::quat(Rotation()); }

glm::vec3 Position3D::Scale() const
{ return m_Scale; }

glm::vec3 Position3D::Front() const
{ return (Quaternion() * World::Front()); }

glm::vec3 Position3D::Right() const
{ return (Quaternion() * World::Right()); }

glm::vec3 Position3D::Up() const
{ return (Quaternion() * World::Up()); }
