#include "mesh_instance.hpp"
#include "theatre/parser/thing_data.hpp"

void MeshInstance::SetVariables(Farg<ThingData> data)
{
    Device::SetVariables(data);

    int render_layers{mRenderLayers.get()};

    data.GetVariable(mMeshID, "Mesh");
    data.GetVariable(mMaterialID, "Material");
    if(data.GetVariable(render_layers, "RenderLayers", "Layers"))
        { mRenderLayers.set(render_layers); }
}

Shared<ThingData> MeshInstance::GetVariables() const
{
    auto data{Device::GetVariables()};

    data->AddVariable(mMeshID, "Mesh");
    data->AddVariable(mMaterialID, "Material");
    data->AddVariable(mRenderLayers.get(), "RenderLayers");

    return data;
}

ID MeshInstance::MeshID() const
{ return mMeshID; }

ID MeshInstance::MaterialID() const
{ return mMaterialID; }

void MeshInstance::MeshID(ID inID)
{ mMeshID = inID; }

void MeshInstance::MaterialID(ID inID)
{ mMaterialID = inID; }
