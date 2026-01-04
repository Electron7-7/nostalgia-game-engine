#include "mesh_instance.hpp"
#include "core/uid.hpp"
#include "theatre/parser/thing_data.hpp"

void __mesh_instance_t::SetVariables(Farg<ThingData> data)
{
    Device::SetVariables(data);

    int render_layers{mRenderLayers.get()};

    data.GetVariable(mMeshID, "Mesh");
    data.GetVariable(mMaterialID, "Material");
    if(data.GetVariable(render_layers, "RenderLayers", "Layers"))
        { mRenderLayers.set(render_layers); }
}

Shared<ThingData> __mesh_instance_t::GetVariables() const
{
    auto data{Device::GetVariables()};

    data->AddVariable(mMeshID, "Mesh");
    data->AddVariable(mMaterialID, "Material");
    data->AddVariable(mRenderLayers.get(), "RenderLayers");

    return data;
}

ID __mesh_instance_t::MeshID() const
{
    return (mMeshID.invalid())
        ? UID::m_Error
        : mMeshID;
}

ID __mesh_instance_t::MaterialID() const
{ return mMaterialID; }

void __mesh_instance_t::MeshID(ID inID)
{ mMeshID = inID; }

void __mesh_instance_t::MaterialID(ID inID)
{ mMaterialID = inID; }

void MeshInstance3D::SetVariables(Farg<ThingData> data)
{
    __mesh_instance_t::SetVariables(data);
    SetTransformVariables(data);
}

Shared<ThingData> MeshInstance3D::GetVariables() const
{
    auto data{__mesh_instance_t::GetVariables()};
    GetTransformVariables(data);
    return data;
}
