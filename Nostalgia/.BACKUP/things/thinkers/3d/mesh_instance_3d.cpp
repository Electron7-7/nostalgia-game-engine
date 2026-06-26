#include "./mesh_instance_3d.hpp"
#include "things/thing_data.hpp"

using namespace TheatreFile;

void MeshInstance3D::InitVariables()
{
    Super::InitVariables();
}

void MeshInstance3D::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);

    data.get_variable(mMesh, "Mesh");
    data.get_variable(mMaterialOverride, "MaterialOverride", "Material");
    data.get_variable(mWireframe, "Wireframe", "WireFrame");
}

Shared<ThingData> MeshInstance3D::GetVariables() const
{
    auto data{Super::GetVariables()};

    data->set_variable(mMesh, "Mesh");
    data->set_variable(mMaterialOverride, "MaterialOverride");
    data->set_variable(mWireframe, "Wireframe");

    return data;
}

Shared<Mesh> MeshInstance3D::GetMesh() const
{ return mMesh; }

Shared<Material> MeshInstance3D::GetMaterialOverride() const
{ return mMaterialOverride; }

bool MeshInstance3D::Wireframe() const
{ return mWireframe; }

void MeshInstance3D::SetMesh(Shared<Mesh> inMesh)
{ mMesh = inMesh; }

void MeshInstance3D::SetMaterialOverride(Shared<Material> inMaterial)
{ mMaterialOverride = inMaterial; }

void MeshInstance3D::SetWireframe(bool inWireframe)
{ mWireframe = inWireframe; }
