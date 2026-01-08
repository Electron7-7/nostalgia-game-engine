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

void MeshInstance3D::MeshID(ID inID)
{ mMeshID = inID; }

ID MeshInstance3D::MaterialOverrideID() const
{ return mMaterialOverrideID; }

void MeshInstance3D::MaterialOverrideID(ID inID)
{ mMaterialOverrideID = inID; }

bool MeshInstance3D::Wireframe() const
{ return mWireframe; }

void MeshInstance3D::Wireframe(bool inWireframe)
{ mWireframe = inWireframe; }
