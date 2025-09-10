#include "actor.hpp"

void Actor::SetupVariables(const data_t& data)
{
    // Thing::SetupVariables(data); // Currently, this doesn't do anything

    data.GetNumber(m_Origin,   "Origin");
    data.GetNumber(m_Rotation, "Rotation");
}

void Actor::SetOrigin(const glm::vec3& origin)
{ m_Origin = origin; }

void Actor::SetRotation(const glm::quat& rotation)
{ m_Rotation = rotation; }

const glm::vec3& Actor::Origin() const
{ return m_Origin; }

const glm::quat& Actor::Rotation() const
{ return m_Rotation; }
