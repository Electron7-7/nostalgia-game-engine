#include "./mesh.hpp"
#include "things/thing_data.hpp"
#include "rendering/vertex_array.hpp"

using namespace TheatreFile;

void Mesh::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);

    data.get_variable(mMaterialID, "Material");
}

Shared<ThingData> Mesh::GetVariables() const
{
    auto data{Super::GetVariables()};

    data->set_variable(mMaterialID, "Material");

    return data;
}

Shared<VertexArray> Mesh::MeshData() const
{ return VertexArray::Create(); }

ID Mesh::MaterialID() const
{ return mMaterialID; }

void Mesh::MaterialID(ID inID)
{ mMaterialID = inID; }
