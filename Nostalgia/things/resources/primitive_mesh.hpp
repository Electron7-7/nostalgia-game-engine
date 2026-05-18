#ifndef PRIMITIVE_MESH_H
#define PRIMITIVE_MESH_H

#include <Nostalgia/things/resources/array_mesh.hpp>

class PrimitiveMesh : public Mesh
{
public:
    THING_CLASS(PrimitiveMesh, Mesh)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    Shared<Material> GetMaterial() const;
    void SetMaterial(Shared<Material>);

    Shared<ArrayMesh> GetArrayMesh() const;
    void UpdateMesh() const;

    int SurfaceCount() const final { return 1; }
    Shared<VertexArray> SurfaceGetVertexArray(int inIndex) const final;
    Shared<Material> SurfaceGetMaterial(int inIndex) const final;

protected:
    mutable Shared<ArrayMesh> m_pArrayMesh{ArrayMesh::Invalid()};

    virtual MeshData_t CreateMeshData() const { return {}; }
};

class BoxMesh : public PrimitiveMesh
{
public:
    THING_CLASS(BoxMesh, PrimitiveMesh)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    glm::vec3 GetSize() const;
    void SetSize(Farg<glm::vec3>);

protected:
    glm::vec3 mSize{2.0f};

    MeshData_t CreateMeshData() const final;
};

class SphereMesh : public PrimitiveMesh
{
public:
    THING_CLASS(SphereMesh, PrimitiveMesh)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    float GetHeight() const;
    void SetHeight(float);

    float GetRadius() const;
    void SetRadius(float);

    bool GetInvertNormals() const;
    void SetInvertNormals(bool);

    uint GetHorizontalResolution() const;
    void SetHorizontalResolution(uint);

    uint GetVerticalResolution() const;
    void SetVerticalResolution(uint);

protected:
    float mHeight{2.0f}, mRadius{1.0f};
    uint mSectors{32}, mStacks{16};
    bool mInvertNormals{false};

    MeshData_t CreateMeshData() const final;
};

/*class CapsuleMesh : public PrimitiveMesh
{
public:
    THING_CLASS(CapsuleMesh, PrimitiveMesh)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    float GetHeight() const;
    void SetHeight(float);

    float GetRadius() const;
    void SetRadius(float);

protected:
    float mHeight{2.0f}, mRadius{0.5f};

    MeshData_t CreateMeshData() const final;
};*/

class CylinderMesh : public PrimitiveMesh
{
public:
    THING_CLASS(CylinderMesh, PrimitiveMesh)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    float GetHeight() const;
    void SetHeight(float);

    float GetBottomRadius() const;
    void SetBottomRadius(float);

    float GetTopRadius() const;
    void SetTopRadius(float);

    uint GetResolution() const;
    void SetResolution(uint);

protected:
    float mHeight{2.0f}, mBottomRadius{0.5f}, mTopRadius{0.5f};
    uint mResolution{16};

    MeshData_t CreateMeshData() const final;
};

#endif // PRIMITIVE_MESH_H
