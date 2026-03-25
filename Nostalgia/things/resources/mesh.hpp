#ifndef MESH_H
#define MESH_H

#include <Nostalgia/fwd/rendering.hpp>

class Mesh : public Resource
{
public:
    Mesh(Shared<FileData> inModel) noexcept;

    SUPER(Resource)
    READY_OVERRIDE
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    virtual Farg<Shared<VertexArray>> MeshData() const;
    virtual void MeshData(Shared<VertexArray>);

    virtual ID MaterialID() const;
    virtual void MaterialID(ID);

protected:
    Shared<FileData> m_pModel{MakeShared<FileData>()};
    Shared<VertexArray> mVertexArray{nullptr};
    ID mMaterialID{};
};

#endif // MESH_H
