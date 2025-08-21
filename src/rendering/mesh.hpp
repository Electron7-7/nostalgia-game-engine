#ifndef MESH_H
#define MESH_H

#include "vertex.hpp"
#include "math/unsigned_int_vector.hpp"

#include <glm/glm.hpp>
#include <vector>
#include <map>

struct Mesh
{
public:
    typedef unsigned int Index;

    Mesh();
    Mesh(const std::vector<Vertex>& Vertices);
    Mesh(const std::vector<Vertex>& Vertices, const std::vector<Index>& Indices);
    Mesh(const std::vector<Vertex>& Vertices, const std::vector<uintvec3>& Faces);

    void AddVertex(Vertex NewVertex);
    void RemoveVertex(Index IndexOfVertexToRemove);
    void RemoveVertex(Vertex VertexToRemove); // Much less efficient, but more direct
    void AddIndex(Index NewIndex);
    void AddFace(uintvec3 NewFaceIndices);

    std::vector<float> GetVertexData() const;
    std::vector<Index> GetIndices() const;

private:
    std::map<Index, Vertex> m_Vertices = {};
    std::vector<Index> m_FaceIndices = {};
};

#endif // MESH_H
