#include "mesh.hpp"

Mesh::Mesh() = default;

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<Index>& indices)
: m_Vertices(vertices), m_Indices(indices)
{}

Mesh::Mesh(const std::vector<Vertex>& vertices)
: m_Vertices(vertices)
{
    for(size_t i = 0 ; i < m_Vertices.size() ; ++i)
    { m_Indices.push_back(i); }
}

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uintvec3>& faces)
: m_Vertices(vertices)
{ AddIndices(faces); }

void Mesh::ClearMeshData()
{
    m_Vertices.clear();
    m_Indices.clear();
}

void Mesh::AddVertices(const std::vector<Vertex>& vertices)
{ m_Vertices.insert(m_Vertices.end(), vertices.begin(), vertices.end()); }

void Mesh::AddIndices(const std::vector<Index>& indices)
{ m_Indices.insert(m_Indices.end(), indices.begin(), indices.end()); }

void Mesh::AddIndices(const std::vector<uintvec3>& faces)
{
    for(const uintvec3& face : faces)
    { m_Indices.insert(m_Indices.end(), {face[0], face[1], face[2]}); }
}

void Mesh::AddVertex(Vertex vertex)
{ m_Vertices.push_back(vertex); }

void Mesh::AddIndex(Index index)
{ m_Indices.insert(m_Indices.end(), index); }

void Mesh::AddFace(uintvec3 face_indices)
{ m_Indices.insert(m_Indices.end(), {face_indices[0], face_indices[1], face_indices[2]}); }

const std::vector<Vertex>& Mesh::GetVertices() const
{ return m_Vertices; }

const std::vector<Mesh::Index>& Mesh::GetIndices() const
{ return m_Indices; }

void Mesh::GetVertexData(std::vector<Vertex>* vertices, std::vector<float>* output_vertex_data)
{
    for(Vertex& vertex : *vertices)
    {
        std::array<float, Vertex::Stride> vertex_data = vertex.VertexData();
        output_vertex_data->insert(output_vertex_data->end(), vertex_data.begin(), vertex_data.end());
    }
}
