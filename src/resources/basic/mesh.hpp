#ifndef MESH_H
#define MESH_H

#include "../resource.hpp"
#include "rendering/vertex.hpp"
#include "math/unsigned_int_vector.hpp"

#include <glm/glm.hpp>
#include <vector>

struct Mesh : public Resource
{
public:
    typedef unsigned int Index;

    Mesh();
    Mesh(const std::vector<Vertex>& Vertices);
    Mesh(const std::vector<Vertex>& Vertices, const std::vector<Index>& Indices);
    Mesh(const std::vector<Vertex>& Vertices, const std::vector<uintvec3>& Faces);

    void ClearMeshData();

    void AddVertices(const std::vector<Vertex>& Vertices);
    void AddIndices(const std::vector<Index>& Indices);
    void AddIndices(const std::vector<uintvec3>& Faces);

    void AddVertex(Vertex NewVertex);
    void RemoveVertex(Index IndexOfVertexToRemove);
    void RemoveVertex(Vertex VertexToRemove); // Much less efficient, but more direct
    void AddIndex(Index NewIndex);
    void AddFace(uintvec3 NewFaceIndices);

    const std::vector<Vertex>& GetVertices() const;
    const std::vector<Index>& GetIndices() const;

    static void GetVertexData(std::vector<Vertex>* Vertices, std::vector<float>* VertexData);

private:
    std::vector<Vertex> m_Vertices = {};
    std::vector<Index>  m_Indices  = {};
};

#endif // MESH_H
