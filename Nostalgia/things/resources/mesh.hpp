#ifndef MESH_H
#define MESH_H

#include <Nostalgia/fwd/rendering.hpp>

class Mesh : public Resource
{
public:
	SET_SUPER(Resource)
	SET_TYPEID(ThingType::Mesh)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    enum ModelType : int
    { MODEL_OBJ };

    static Shared<Mesh> CreateFromMemory(const uchar* inData, size_t inSize, ModelType inType);

    virtual Farg<Shared<VertexArray>> MeshData() const;
    virtual void MeshData(Shared<VertexArray>);

    virtual ID MaterialID() const;
    virtual void MaterialID(ID);

protected:
    Shared<FileData> m_pModel{MakeShared<FileData>()};
    Shared<VertexArray> mVertexArray{nullptr};
    ID mMaterialID{};

    Error LoadModel();
};

#endif // MESH_H
