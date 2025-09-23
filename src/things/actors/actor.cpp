#include "actor.hpp"
#include "theatre/data_t.hpp"

void Actor::SetupVariables(const data_t& data)
{
    Thing::SetupVariables(data); // Currently, this doesn't do anything

    data.GetNumber(m_Origin, "Origin");
    data.GetNumber(m_Scale, "Scale");
    glm::vec3 euler;
    if(data.GetNumber(euler, "RotationDegrees"))
        { Euler(euler, true); }
    if(data.GetNumber(euler, "Rotation"))
        { Euler(euler); }
    glm::quat quat;
    if(data.GetNumber(quat, "Quaternion"))
        { Quaternion(quat); }
    data.GetTheatreRef(m_MeshInstanceID, "MeshInstance");
    data.GetBool(m_Visible, "Visible");
    data.GetBool(m_Wireframe, "MakeWireframe");
    data.GetBool(m_Wireframe, "Wireframe");
}

id_t Actor::GetMeshInstanceID() const
{ return m_MeshInstanceID; }

void Actor::SetMeshInstanceID(id_t mesh_instance_id)
{ m_MeshInstanceID = mesh_instance_id; }
