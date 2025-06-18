#include "mesh.hpp"
#include "common/debugging.hpp"

void Mesh::AddVertex(Vertex vertex)
{ _vertices[_vertices.size()] = vertex; }

void Mesh::AddIndex(Mesh::Index index)
{ _face_indices.insert(_face_indices.end(), index); }

void Mesh::RemoveVertex(Mesh::Index index_of_vertex_to_remove)
{
    if(_vertices.contains(index_of_vertex_to_remove))
        _vertices.erase(index_of_vertex_to_remove);
    else
        PRINTWARN("Mesh::RemoveVertex(Index) - No index matching the argument was found in this mesh");
}

void Mesh::RemoveVertex(Vertex vertex_to_remove) // Much less efficient, but more direct
{
    for(auto& [index, vertex] : _vertices)
        if(vertex == vertex_to_remove)
        {
            _vertices.erase(index);
            return;
        }

    PRINTWARN("Mesh::RemoveVertex(Vertex) - No vertex matching the argument was found in this mesh");
}

std::vector<float> Mesh::GetVertexData() const
{
    std::vector<float> all_vertex_data = {};
    for(auto& [index, vertex] : _vertices)
    {
        // FIXME: change hardcoded '11' to a variable that helps identify the vertex data stride
        std::array<float, 11> iterative_vertex_data = vertex.VertexData();
        all_vertex_data.insert(all_vertex_data.end(), iterative_vertex_data.begin(), iterative_vertex_data.end());
    }

    return all_vertex_data;
}

std::vector<Mesh::Index> Mesh::GetIndices() const
{ return _face_indices; }
