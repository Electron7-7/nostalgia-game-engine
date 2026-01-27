#include "mesh_instance_3d.hpp"

using namespace TheatreFile;

void MeshInstance3D::SetVariables(Farg<ThingData> data)
{
    Visual3D::SetVariables(data);

    data.get_variable(mMeshID, "Mesh");
    data.get_variable(mMaterialOverrideID, "MaterialOverride", "Material");
    data.get_variable(mWireframe, "Wireframe", "WireFrame");
}

Shared<ThingData> MeshInstance3D::GetVariables() const
{
    auto data{Visual3D::GetVariables()};

    data->set_variable(mMeshID, "Mesh");
    data->set_variable(mMaterialOverrideID, "MaterialOverride");
    data->set_variable(mWireframe, "Wireframe");

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
