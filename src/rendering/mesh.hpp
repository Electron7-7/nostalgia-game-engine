#ifndef MESH_H
#define MESH_H

#include "vertex.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <map>

struct Mesh
{
public:
    typedef unsigned int Index;

    Mesh();
    Mesh(const std::string& Name, const std::vector<Vertex>& Vertices);
    Mesh(const std::string& Name, const std::vector<Vertex>& Vertices, const std::vector<Index>& Indices);

    void AddVertex(Vertex NewVertex);
    void RemoveVertex(Index IndexOfVertexToRemove);
    void RemoveVertex(Vertex VertexToRemove); // Much less efficient, but more direct
    void AddIndex(Index NewIndex);
    void SetName(const std::string& NewName);

    std::vector<float> GetVertexData() const;
    std::vector<Index> GetIndices() const;
    std::string GetName() const;

private:
    std::string _mesh_name = "UNDEFINED";
    std::map<Index, Vertex> _vertices = {};
    std::vector<Index> _face_indices = {};
};

#endif // MESH_H
