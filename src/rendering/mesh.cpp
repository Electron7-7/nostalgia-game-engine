#include "mesh.hpp"
#include "common/debugging.hpp"

Mesh::Mesh() = default;

Mesh::Mesh(const std::string& init_name, const std::vector<Vertex>& init_vertices, const std::vector<Index>& init_indices)
: _mesh_name(init_name), _face_indices(init_indices)
{
    for(unsigned int i = 0 ; i < init_vertices.size() ; i++)
        _vertices[i] = init_vertices.at(i);
}

Mesh::Mesh(const std::string& init_name, const std::vector<Vertex>& init_vertices)
: Mesh(init_name, init_vertices, std::vector<Index>{})
{}

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

void Mesh::SetName(const std::string& name)
{ _mesh_name = name; }

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

std::string Mesh::GetName() const
{ return _mesh_name; }
