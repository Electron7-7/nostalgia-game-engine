#include "actor.hpp"
#include "theatre/data_t.hpp"

void Actor::SetupVariables(const data_t& data)
{
    Thing::SetupVariables(data); // Currently, this doesn't do anything

    data.GetNumber(m_Origin, "Origin");
    data.GetNumber(m_Scale, "Scale");
    if(data.GetNumber(m_Euler, "RotationDegrees"))
        { m_Quaternion = glm::quat(glm::radians(m_Euler)); }
    if(data.GetNumber(m_Euler, "Rotation"))
        { m_Quaternion = glm::quat(m_Euler); }
    if(data.GetNumber(m_Quaternion, "Quaternion"))
        { m_Euler = glm::eulerAngles(m_Quaternion); }
    data.GetTheatreRef(m_MeshInstanceID, "MeshInstance");
    data.GetBool(m_Visible, "Visible");
    data.GetBool(m_Wireframe, "MakeWireframe");
    data.GetBool(m_Wireframe, "Wireframe");
}

id_t Actor::GetMeshInstanceID() const
{ return m_MeshInstanceID; }

void Actor::SetMeshInstanceID(id_t mesh_instance_id)
{ m_MeshInstanceID = mesh_instance_id; }

bool Actor::Visible() const
{ return m_Visible; }
