#include "prototype_actor.hpp"

void PrototypeActor::SetupVariables(const data_t& data)
{
    Actor::SetupVariables(data);

    data.GetTheatreRef(m_MeshInstanceID, "MeshInstance");
}

id_t PrototypeActor::GetMeshInstance() const
{ return m_MeshInstanceID; }

bool PrototypeActor::HasMeshInstance() const
{ return (m_MeshInstanceID != NoID); }

void PrototypeActor::SetMeshInstance(id_t mesh_instance_id)
{ m_MeshInstanceID = mesh_instance_id; }
