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
    static FileType ConvertToFileType(ArrayMesh::ModelFileType);

    virtual Error LoadModelData(const uchar* inData, size_t inSize, ModelFileType inType);
    virtual Error LoadModelFile(Sarg inFilePath);
    virtual bool LoadedFromFile() const;
    virtual Sarg ModelFilePath() const;

    virtual void AddSurface(PrimitiveType, Farg<MeshData_t>, Farg<Indices_t> = {}) override;
    virtual void ClearSurfaces() override;
    virtual void RemoveSurface(int inIndex) override;
    virtual int SurfaceCount() const override;
    virtual PrimitiveType SurfaceGetPrimitive(int inIndex) const override;
    virtual Shared<VertexArray> SurfaceGetVertexArray(int inIndex) const override;
    // TODO: implement per-surface materials
    virtual Shared<Material> SurfaceGetMaterial(int inIndex) const override;

    // TODO: remove this and replace it with per-surface materials
    Shared<Material> mMaterial{MakeShared<Material>()};

protected:
    std::string mModelFilepath{""};
    std::vector<Surface> mSurfaces{};

    Error CreateMeshData(Farg<FileData>);
    Error CreateMeshFromOBJ(Farg<FileData>);
};

#endif // ARRAY_MESH_H
