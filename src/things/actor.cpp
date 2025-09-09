#include "actor.hpp"

void Actor::SetOrigin(const glm::vec3& origin)
{ m_Origin = origin; }

void Actor::SetRotation(const glm::quat& rotation)
{ m_Rotation = rotation; }

const glm::vec3& Actor::Origin() const
{ return m_Origin; }

const glm::quat& Actor::Rotation() const
{ return m_Rotation; }
