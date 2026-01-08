#include "mesh_instance_3d.hpp"
#include "core/uid.hpp"
#include "theatre/parser/thing_data.hpp"

void MeshInstance3D::SetVariables(Farg<ThingData> data)
{
    Actor3D::SetVariables(data);

    data.GetVariable(mMeshID, "Mesh");
    data.GetVariable(mMaterialOverrideID, "MaterialOverride", "Material");
    data.GetVariable(mWireframe, "Wireframe", "WireFrame");
}

Shared<ThingData> MeshInstance3D::GetVariables() const
{
    auto data{Actor3D::GetVariables()};

    data->AddVariable(mMeshID, "Mesh");
    data->AddVariable(mMaterialOverrideID, "MaterialOverride");
    data->AddVariable(mWireframe, "Wireframe");

    return data;
}

ID MeshInstance3D::MeshID() const
{
    return (mMeshID.invalid())
        ? UID::m_Error
        : mMeshID;
}

ID MeshInstance3D::MaterialOverrideID() const
{ return mMaterialOverrideID; }

bool MeshInstance3D::Wireframe() const
{ return mWireframe; }

void MeshInstance3D::SetMeshID(ID inID)
{ mMeshID = inID; }

void MeshInstance3D::SetMaterialOverrideID(ID inID)
{ mMaterialOverrideID = inID; }

void MeshInstance3D::SetWireframe(bool inWireframe)
{ mWireframe = inWireframe; }
