#include "actor.hpp"

void Actor::SetupVariables(const data_t& data)
{
    // Thing::SetupVariables(data); // Currently, this doesn't do anything

    data.GetNumber(Position3D::m_Origin,   "Origin");
    data.GetNumber(Position3D::m_Rotation, "Rotation");
}
