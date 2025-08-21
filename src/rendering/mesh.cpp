#include "mesh.hpp"
#include "printing.hpp"

Mesh::Mesh() = default;

Mesh::Mesh(const std::vector<Vertex>& init_vertices, const std::vector<Index>& init_indices)
: m_FaceIndices(init_indices)
{
    for(unsigned int i = 0 ; i < init_vertices.size() ; i++)
    { m_Vertices[i] = init_vertices.at(i); }
}

Mesh::Mesh(const std::vector<Vertex>& init_vertices)
: Mesh(init_vertices, std::vector<Index>{})
{}

Mesh::Mesh(const std::vector<Vertex>& init_vertices, const std::vector<uintvec3>& init_faces)
: Mesh(init_vertices)
{
    for(uintvec3 face : init_faces)
    { m_FaceIndices.insert(m_FaceIndices.end(), {face[0], face[1], face[2]}); }
}

void Mesh::AddVertex(Vertex vertex)
{ m_Vertices[m_Vertices.size()] = vertex; }

void Mesh::AddIndex(Mesh::Index index)
{ m_FaceIndices.insert(m_FaceIndices.end(), index); }

void Mesh::AddFace(uintvec3 face_indices)
{ m_FaceIndices.insert(m_FaceIndices.end(), {face_indices[0], face_indices[1], face_indices[2]}); }

void Mesh::RemoveVertex(Mesh::Index index_of_vertex_to_remove)
{
    if(m_Vertices.contains(index_of_vertex_to_remove))
    { m_Vertices.erase(index_of_vertex_to_remove); }
    else
    { PRINT_WARNING("Mesh::RemoveVertex(Index) - No index matching the argument was found in this mesh") }
}

void Mesh::RemoveVertex(Vertex vertex_to_remove) // Much less efficient, but more direct
{
    for(auto& [index, vertex] : m_Vertices)
    {
        if(vertex == vertex_to_remove)
        {
            m_Vertices.erase(index);
            return;
       }
    }

    PRINT_WARNING("Mesh::RemoveVertex(Vertex) - No vertex matching the argument was found in this mesh")
}

std::vector<float> Mesh::GetVertexData() const
{
    std::vector<float> all_vertex_data = {};
    for(auto& [index, vertex] : m_Vertices)
    {
        // FIXME: change hardcoded '11' to a variable that helps identify the vertex data stride
        std::array<float, 11> iterative_vertex_data = vertex.VertexData();
        all_vertex_data.insert(all_vertex_data.end(), iterative_vertex_data.begin(), iterative_vertex_data.end());
    }

    return all_vertex_data;
}

std::vector<Mesh::Index> Mesh::GetIndices() const
{ return m_FaceIndices; }
