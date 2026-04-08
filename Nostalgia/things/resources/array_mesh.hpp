#ifndef ARRAY_MESH_H
#define ARRAY_MESH_H

#include <Nostalgia/things/resources/mesh.hpp>
#include <Nostalgia/rendering/vertex_array.hpp>

class ArrayMesh : public Mesh
{
public:
    SET_SUPER(Mesh)
    SET_TYPEID(ThingType::ArrayMesh)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    enum ModelFileType : int { MODEL_OBJ };

    static Shared<ArrayMesh> CreateFromMemory(const uchar* inData, size_t inSize, ModelFileType inType);
    static Shared<ArrayMesh> CreateFromFile(Sarg inFilePath);

    virtual Error LoadModelData(const uchar* inData, size_t inSize, ModelFileType inType);
    virtual Error LoadModelFile(Sarg inFilePath);

    virtual Shared<VertexArray> MeshData() const override;

    virtual bool LoadedFromFile() const;
    virtual Sarg ModelFilePath() const;

    ID MaterialID() const { return mMaterialID; }
    void MaterialID(ID inUID) { mMaterialID = inUID; }

protected:
    std::string mModelFilepath{""};
    Shared<VertexArray> m_pVertexArray{VertexArray::Create()};

    virtual Error CreateMeshData(Farg<FileData> inModelFile);

private:
    ID mMaterialID{};
};

#endif // ARRAY_MESH_H
