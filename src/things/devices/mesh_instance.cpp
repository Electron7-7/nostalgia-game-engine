#include "mesh_instance.hpp"
#include "theatre/thing_data.hpp"

void MeshInstance::SetupVariables(const ThingData& data)
{
    Device::SetupVariables(data);

    data.GetReference(mMeshID, "Mesh");
    data.GetReference(mMaterialID, "Material");
}

ID MeshInstance::GetMeshID() const
{ return mMeshID; }

ID MeshInstance::GetMaterialID() const
{ return mMaterialID; }
