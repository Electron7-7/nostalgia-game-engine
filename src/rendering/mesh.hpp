#ifndef MESH_H
#define MESH_H

#include "vertex.hpp"
#include "common/debugging.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <map>

struct Mesh
{
public:
    typedef unsigned int Index;

    std::vector<float> GetVertexData() const
    {
        std::vector<float> all_vertex_data = {};
        for(auto& [index, vertex] : vertices)
        {
            std::array<float, 11> iterative_vertex_data = vertex.VertexData();
            all_vertex_data.insert(all_vertex_data.end(), iterative_vertex_data.begin(), iterative_vertex_data.end());
        }

        return all_vertex_data;
    }

    std::vector<Index> GetIndices() const
    { return face_indices; }

    void AddVertex(Vertex NewVertex)
    { vertices[vertices.size()] = NewVertex; }

    void AddIndex(Index NewIndex)
    { face_indices.insert(face_indices.end(), NewIndex); }

    void RemoveVertex(Index IndexOfVertexToRemove)
    {
        if(vertices.contains(IndexOfVertexToRemove))
            vertices.erase(IndexOfVertexToRemove);
        else
            PRINTWARN("Mesh::RemoveVertex(Index) - No index matching the argument was found in this mesh");
    }

    void RemoveVertex(Vertex VertexToRemove) // Much less efficient, but more direct
    {
        for(auto& [index, vertex] : vertices)
            if(vertex == VertexToRemove)
            {
                vertices.erase(index);
                return;
            }

        PRINTWARN("Mesh::RemoveVertex(Vertex) - No vertex matching the argument was found in this mesh");
    }

private:
    std::map<Index, Vertex> vertices = {};
    std::vector<Index> face_indices;
};

#endif // MESH_H
