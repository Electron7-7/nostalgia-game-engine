#ifndef MESH_H
#define MESH_H

#include "../resource.hpp"
#include "safe_return.hpp"
#include "rendering/vertex.hpp"
#include "math/unsigned_int_vector.hpp"

#include <glm/glm.hpp>
#include <vector>

class Mesh : public Resource
{
public:
    typedef unsigned int Index;

    using Resource::Resource;
    virtual void SetupVariables(const data_t&);
    virtual SafeStatus CreateResource();

    void clear();

    const std::vector<Vertex>& GetVertices() const;
    const std::vector<Index>& GetIndices() const;
    std::vector<float> GetVertexData();

private:
    std::vector<Vertex> m_Vertices = {};
    std::vector<Index>  m_Indices  = {};

    SafeStatus try_CreateOBJMesh();

    void AddVertices(const std::vector<Vertex>& Vertices);
    void AddIndices(const std::vector<Index>& Indices);
    void AddIndices(const std::vector<uintvec3>& Faces);

    void AddVertex(Vertex NewVertex);
    void AddIndex(Index NewIndex);
    void AddFace(uintvec3 NewFaceIndices);
};

#endif // MESH_H
