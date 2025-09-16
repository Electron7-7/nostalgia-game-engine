#include "position_3d.hpp"
#include "world.hpp"

void Position3D::SetOrigin(const glm::vec3& Origin)
{ m_Origin = Origin; }

void Position3D::SetRotation(const glm::quat& Rotation)
{ m_Rotation = Rotation; }

const glm::vec3& Position3D::Origin() const
{ return m_Origin; }

const glm::quat& Position3D::Rotation() const
{ return m_Rotation; }

glm::vec3 Position3D::Front() const
{ return (m_Rotation * World::Front()); }

glm::vec3 Position3D::Right() const
{ return (m_Rotation * World::Right()); }

glm::vec3 Position3D::Up() const
{ return (m_Rotation * World::Up()); }
