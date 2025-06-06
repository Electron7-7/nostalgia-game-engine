#ifndef MESH_H
#define MESH_H

#include "vertex.hpp"
#include "debugging.hpp"
#include <glm/glm.hpp>
#include <map>

struct Mesh
{
public:
    typedef unsigned int Index;

    void AddVertex(Vertex NewVertex)
    {
        vertices[vertices.size()] = NewVertex;
    }

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
    // std::vector<Tri> tris = {}; // TODO: Decide whether or not to make Tris their own class
};

#endif // MESH_H
