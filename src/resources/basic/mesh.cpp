#include "mesh.hpp"
#include "resources/data/models.hpp"
#include "filesystem/filesystem.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_USE_MAPBOX_EARCUT
#include "TinyOBJLoader/tiny_obj_loader.h"

#include <map>

static std::map<std::string, ModelType> s_FileTypesByExtension
{
    { ".obj", ModelType::OBJ }
};

Mesh Mesh::Error  = Mesh(model_OBJ_Error, ModelType::OBJ);
Mesh Mesh::Ramiel = Mesh(model_OBJ_Ramiel, ModelType::OBJ);

Mesh::Mesh() = default;

Mesh::Mesh(const std::string& model_data, ModelType model_type)
: m_FileType(model_type), m_FileName(""), m_FileData(model_data)
{}

void Mesh::LoadModelFile(const std::string& file_path, ModelType file_type)
{
    m_FileType = file_type;
    m_FileName = file_path;
    m_FileData.clear();
}

void Mesh::LoadModelData(const std::string& file_data, ModelType file_type)
{
    m_FileType = file_type;
    m_FileData = file_data;
    m_FileName.clear();
}

void Mesh::clear()
{
    m_Vertices.clear();
    m_Indices.clear();
}

const std::vector<Vertex>& Mesh::GetVertices() const
{
    if(m_MeshDataStatus == ResourceStatus::FAILED)
        { return Mesh::Error.m_Vertices; }
    return m_Vertices;
}

const std::vector<Mesh::Index>& Mesh::GetIndices() const
{
    if(m_MeshDataStatus == ResourceStatus::FAILED)
        { return Mesh::Error.m_Indices; }
    return m_Indices;
}

void Mesh::GetVertexData(std::vector<Vertex>* vertices, std::vector<float>* output_vertex_data)
{
    for(Vertex& vertex : *vertices)
    {
        std::array<float, Vertex::Stride> vertex_data = vertex.VertexData();
        output_vertex_data->insert(output_vertex_data->end(), vertex_data.begin(), vertex_data.end());
    }
}

SafeStatus Mesh::CreateMeshData()
{
    if(m_MeshDataStatus != ResourceStatus::NOT_PROCESSED)
        { return Status::NO_ERR; }

    if(m_FileData.empty())
    {
        SafeStatus read_status = Filesystem::try_ReadFileToString(m_FileName, m_FileData);
        if(read_status != Status::NO_ERR)
            { return read_status; }
    }

    if(m_FileType == ModelType::Unknown)
    {
        std::string file_extension = Filesystem::GetExtension(m_FileName);
        if(s_FileTypesByExtension.contains(file_extension))
            { m_FileType = s_FileTypesByExtension.at(file_extension); }
    }

    SafeStatus status = Status::NO_ERR;

    switch(m_FileType)
    {
    case ModelType::OBJ:
        status = try_CreateOBJMesh();
    default:
        status = Status::MeshUNKNOWN_FILETYPE;
    }

    m_MeshDataStatus = ResourceStatus::SUCCESSFUL;

    if(status != Status::NO_ERR)
    {
        m_MeshDataStatus = ResourceStatus::FAILED;
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
{ m_Indices.insert(m_Indices.end(), index); }

void Mesh::AddFace(uintvec3 face_indices)
{ m_Indices.insert(m_Indices.end(), {face_indices[0], face_indices[1], face_indices[2]}); }

// Taken from GraphX
SafeStatus Mesh::try_CreateOBJMesh()
{
    tinyobj::ObjReaderConfig reader_config;
    tinyobj::ObjReader reader;

    if(!reader.ParseFromString(m_FileData, "", reader_config))
    {
        if(!reader.Error().empty())
        {
            PRINT_ERROR("TinyObjReader Error: '{}'", reader.Error())
            return Status::MeshFAILED_TO_LOAD_OBJ;
        }
    }

    if(!reader.Warning().empty())
        { PRINT_WARNING("TinyObjReader Warning: '{}'", reader.Warning()) }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();

    m_Vertices.clear();
    m_Indices.clear();

    Vertex temp_vertex;

    // Loop over shapes
    // Shapes are full meshes in the OBJ (there can be multiple)
    for (size_t s = 0; s < shapes.size(); ++s)
    {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); ++f)
        {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; ++v)
            {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
                tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
                tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];

                temp_vertex.SetPosition(glm::vec3((float)vx, (float)vy, (float)vz));

                // Check if `normal_index` is zero or positive. negative = no normal data
                if (idx.normal_index >= 0)
                {
                    tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
                    tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
                    tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];

                    temp_vertex.SetNormal(glm::vec3((float)nx, (float)ny, (float)nz));
                }

                else
                {
                    temp_vertex.SetNormal(glm::vec3(0.0f));
                }

                // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                if (idx.texcoord_index >= 0)
                {
                    tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
                    tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];

                    temp_vertex.SetUV(glm::vec2((float)tx, (float)ty));
                }

                else
                {
                    temp_vertex.SetUV(glm::vec2(0.0f));
                }

                if (idx.texcoord_index >= 0)
                {
                    tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
                    tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
                    tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];

                    temp_vertex.SetColor(glm::vec3((float)red, (float)green, (float)blue));
                }

                else
                {
                    temp_vertex.SetColor(glm::vec3(1.0f));
                }

                AddVertex(temp_vertex);
            }

            index_offset += fv;
        }
    }

    return Status::NO_ERR;
}
