#include "actor.hpp"
#include "theatre/data_t.hpp"

void Actor::SetupVariables(const data_t& data)
{
    Thing::SetupVariables(data); // Currently, this doesn't do anything

    data.GetNumber(m_Origin, "Origin");
    data.GetNumber(m_Scale, "Scale");
    if(data.GetNumber(m_Euler, "Rotation"))
        { Quaternion() = glm::quat(m_Euler); }
    else if(data.GetNumber(m_Euler, "RotationDegrees"))
        { Quaternion() = glm::quat(glm::radians(m_Euler)); }
    data.GetNumber(m_Quaternion, "Quaternion");
    data.GetTheatreRef(m_MeshInstanceID, "MeshInstance");
}

id_t Actor::GetMeshInstanceID() const
{ return m_MeshInstanceID; }

void Actor::SetMeshInstanceID(id_t mesh_instance_id)
{ m_MeshInstanceID = mesh_instance_id; }
