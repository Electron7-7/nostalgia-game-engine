#include "mesh.hpp"
#include "printing.hpp"
#include "theatre/data_t.hpp"
#include "filesystem/file_data.hpp"
#include "managers/backend_manager.hpp"
// #define TINYOBJLOADER_IMPLEMENTATION
// #define TINYOBJLOADER_USE_MAPBOX_EARCUT
// #include "TinyOBJLoader/tiny_obj_loader.h"

// #include <numeric>

void Mesh::SetupVariables(const data_t& data)
{
    Resource::SetupVariables(data);
    CreateResource();
    g_pBackendManager->Graphics()->BufferMesh(&m_FileData, m_ID);
}

void Mesh::clear()
{
    m_Vertices.clear();
    m_Indices.clear();
    m_Status = ResourceStatus::NOT_PROCESSED;
}

const std::vector<Vertex>& Mesh::GetVertices() const
{ return m_Vertices; }

const std::vector<Mesh::Index>& Mesh::GetIndices() const
{ return m_Indices; }

std::vector<float> Mesh::GetVertexData()
{
    std::vector<float> output = {};
    for(const Vertex& vertex : m_Vertices)
        { vertex.GetVertexData(output); }
    return output;
}

SafeStatus Mesh::CreateResource()
{
    if(m_FileData.Status() == DataStatus::FAILED)
        { return Status::ResourceBAD_FILE_DATA; }

    SafeStatus status = Status::NO_ERR;
    m_Status = ResourceStatus::SUCCESSFUL;

    switch(m_FileData.Type())
    {
    case FileType::model_OBJ:
        status = try_CreateOBJMesh();
        break;
    case FileType::Unknown:
    default:
        status = Status::ResourceUNKNOWN_FILETYPE;
        break;
    }

    if(status != Status::NO_ERR)
    {
        PRINT_WARNING("Mesh::CreateResource - Failed to create mesh data for '{}' [ID#{}]", m_Name, m_ID)
        m_Status = ResourceStatus::FAILED;
        clear();
    }
    return status;
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
{ m_Indices.push_back(index); }

void Mesh::AddFace(uintvec3 face_indices)
{ m_Indices.insert(m_Indices.end(), {face_indices[0], face_indices[1], face_indices[2]}); }

// Taken from GraphX
SafeStatus Mesh::try_CreateOBJMesh()
{
    /*tinyobj::ObjReaderConfig reader_config;
    tinyobj::ObjReader reader;

    if(!reader.ParseFromString(m_FileData.String(), "", reader_config))
    {
        if(!reader.Error().empty())
        {
            PRINT_ERROR("Mesh::try_CreateOBJMesh - TinyObjReader Error: '{}'", reader.Error())
            m_Status = ResourceStatus::FAILED;
            return Status::ResourceBAD_FILE_DATA;
        }
    }

    if(!reader.Warning().empty())
        { PRINT_WARNING("TinyObjReader Warning: '{}'", reader.Warning()) }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();

    m_Vertices.clear();
    m_Indices.clear();

    Vertex temp_vertex;

    // This looping code is from https://github.com/tinyobjloader/tinyobjloader
    // Loop over shapes (Shapes are full meshes in the OBJ (there can be multiple))
    for(size_t s = 0; s < shapes.size(); ++s)
    {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for(size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); ++f)
        {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

            // Loop over vertices in the face.
            for(size_t v = 0; v < fv; ++v)
            {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
                tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
                tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];

                temp_vertex.SetPosition(glm::vec3((float)vx, (float)vy, (float)vz));

                // Check if `normal_index` is zero or positive. negative = no normal data
                if(idx.normal_index >= 0)
                {
                    tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
                    tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
                    tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];

                    temp_vertex.SetNormal(glm::vec3((float)nx, (float)ny, (float)nz));
                }
                else
                    { temp_vertex.SetNormal(glm::vec3(0.0f)); }

                // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                if(idx.texcoord_index >= 0)
                {
                    tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
                    tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];

                    temp_vertex.SetUV(glm::vec2((float)tx, (float)ty));
                }
                else
                    { temp_vertex.SetUV(glm::vec2(0.0f)); }

                if(idx.texcoord_index >= 0)
                {
                    tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
                    tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
                    tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];

                    temp_vertex.SetColor(glm::vec3((float)red, (float)green, (float)blue));
                }
                else
                    { temp_vertex.SetColor(glm::vec3(1.0f)); }

                AddVertex(temp_vertex);
            }
            index_offset += fv;
        }
    }

    // Fix OBJ Indicies (if this is OpenGL-specific, move this to the graphics backend)
    m_Indices.resize(m_Vertices.size());
    std::iota(m_Indices.begin(), m_Indices.end(), 0);*/
    return Status::NO_ERR;
}
