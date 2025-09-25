#include "mesh_instance.hpp"
#include "theatre/data_t.hpp"

void MeshInstance::SetupVariables(const data_t& data)
{
    Device::SetupVariables(data);

    data.GetReference(m_MeshID, "Mesh");
    data.GetReference(m_MaterialID, "Material");
}

id_t MeshInstance::GetMeshID() const
{ return m_MeshID; }

id_t MeshInstance::GetMaterialID() const
{ return m_MaterialID; }
