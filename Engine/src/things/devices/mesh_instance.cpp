#include "mesh_instance.hpp"
#include "theatre/parser/thing_data.hpp"

void MeshInstance::SetVariables(const ThingData& data)
{
    Device::SetVariables(data);

    data.GetVariable(mMeshID, "Mesh");
    data.GetVariable(mMaterialID, "Material");
}

ThingData MeshInstance::GetVariables() const
{
    ThingData thing_data{Device::GetVariables()};

    thing_data.AddVariable(mMeshID, "Mesh");
    thing_data.AddVariable(mMaterialID, "Material");

    return thing_data;
}

ID MeshInstance::GetMeshID() const
{ return mMeshID; }

ID MeshInstance::GetMaterialID() const
{ return mMaterialID; }
