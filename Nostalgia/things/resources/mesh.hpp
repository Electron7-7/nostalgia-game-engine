#ifndef MESH_H
#define MESH_H

#include <Nostalgia/fwd/rendering.hpp>
#include <Nostalgia/things/resources/material.hpp>

class Mesh : public Resource
{
public:
    THING_CLASS(Mesh, Resource)

    using Indices_t = std::vector<uint>;
    using VertexAttributes_t = std::vector<float>;
    using MeshData_t = std::vector<VertexAttributes_t>;

    enum ArrayFormat : int
    {
        ARRAY_FORMAT_VERTEX=0,
        ARRAY_FORMAT_COLORS=1,
        ARRAY_FORMAT_NORMALS=2,
        ARRAY_FORMAT_UV=3,
        ARRAY_FORMAT_MAX=4
    };

    enum PrimitiveType : int
    { PRIMITIVE_POINTS, PRIMITIVE_LINES, PRIMITIVE_TRIANGLES, PRIMITIVE_MAX };

    struct Surface
    {
        PrimitiveType primitive{PRIMITIVE_TRIANGLES};
        Shared<VertexArray> vertex_array{nullptr};
        Shared<Material> material_id{nullptr};
    };

    virtual void AddSurface(PrimitiveType, Farg<MeshData_t>, Farg<Indices_t> = {}) {}
    virtual void ClearSurfaces() {}
    virtual void RemoveSurface(int inIndex) {}
    virtual int SurfaceCount() const { return 0; }
    virtual PrimitiveType SurfaceGetPrimitive(int inIndex) const { return PRIMITIVE_TRIANGLES; }
    virtual Shared<VertexArray> SurfaceGetVertexArray(int inIndex) const { return nullptr; }
    virtual Shared<Material> SurfaceGetMaterial(int inIndex) const { return nullptr; }
};


inline void Mesh::InitVariables()
{
    Super::InitVariables();
}

#endif // MESH_H
