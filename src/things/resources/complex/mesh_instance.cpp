#include "mesh_instance.hpp"
#include "theatre/data_t.hpp"

void MeshInstance::SetupVariables(const data_t& data)
{
    Resource::SetupVariables(data);

    data.GetTheatreRef(m_MeshID, "Mesh");
    data.GetTheatreRef(m_MaterialID, "Material");
}

void MeshInstance::AssignMesh(id_t mesh_id)
{ m_MeshID = mesh_id; }

id_t MeshInstance::GetMeshID() const
{ return m_MeshID; }

void MeshInstance::AssignMaterial(id_t Material_id)
{ m_MaterialID = Material_id; }

id_t MeshInstance::GetMaterialID() const
{ return m_MaterialID; }
