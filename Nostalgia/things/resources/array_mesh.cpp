#include "./array_mesh.hpp"
#include "things/thing_factory.hpp"
#include "things/thing_data.hpp"
#include "rendering/mesh_buffers.hpp"
#include "rendering/vertex.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_USE_MAPBOX_EARCUT
#include "thirdparty/TinyOBJLoader/tiny_obj_loader.h"

using namespace TheatreFile;

static FileType s_ModelTypeToFileType(ArrayMesh::ModelFileType);
static Error s_LoadOBJ(Farg<FileData>, Shared<VertexArray>&);

void ArrayMesh::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);
    if(data.get_variable(mModelFilepath, "File", "Model") == OK)
        { LoadModelFile(mModelFilepath); }

    data.get_variable(mMaterialID, "Material");
}

Shared<ThingData> ArrayMesh::GetVariables() const
{
    auto data{Super::GetVariables()};
    data->set_variable(mModelFilepath, "Model");
    data->set_variable(mMaterialID, "Material");
    return data;
}

Shared<ArrayMesh> ArrayMesh::CreateFromMemory(const uchar* inData, size_t inSize, ModelFileType inType)
{
    auto output{DCast<ArrayMesh>(ThingFactory::MakeThing(ThingType::ArrayMesh, "Untitled_ArrayMesh"))};
    print_error_enum(output->LoadModelData(inData, inSize, inType));
    return output;
}

Shared<ArrayMesh> ArrayMesh::CreateFromFile(Sarg inFilePath)
{
    auto output{DCast<ArrayMesh>(ThingFactory::MakeThing(ThingType::ArrayMesh, "Untitled_ArrayMesh"))};
    print_error_enum(output->LoadModelFile(inFilePath));
    return output;
}

#pragma message("TODO: combine duplicate code")
Error ArrayMesh::LoadModelData(const uchar* inData, size_t inSize, ModelFileType inType)
{ return CreateMeshData({inData, inSize, s_ModelTypeToFileType(inType)}); }

Error ArrayMesh::LoadModelFile(Sarg inFilePath)
{ return CreateMeshData({inFilePath}); }

Shared<VertexArray> ArrayMesh::MeshData() const
{ return m_pVertexArray; }

bool ArrayMesh::LoadedFromFile() const
{ return not mModelFilepath.empty(); }

Sarg ArrayMesh::ModelFilePath() const
{ return mModelFilepath; }

FileType s_ModelTypeToFileType(ArrayMesh::ModelFileType inType)
{
    switch(inType)
    {
    case ArrayMesh::MODEL_OBJ:
        return FileType::model_OBJ;
    default:
        print_warning("Invalid/unknown model file type encountered");
        return FileType::Unknown;
    }
}

Error ArrayMesh::CreateMeshData(Farg<FileData> inModelFile)
{
    if(not inModelFile.status())
        { return inModelFile.status(); }

    switch(inModelFile.file_type())
    {
    case FileType::model_OBJ:
        return s_LoadOBJ(inModelFile, m_pVertexArray);
    case FileType::Unknown:
    default:
        print_error("Data of unknown file-type cannot be buffered");
        return ERR_INVALID_TYPE;
    }
    return OK;
}

///////////////////
// MODEL PARSING //
///////////////////
Error s_LoadOBJ(Farg<FileData> inModelFile, Shared<VertexArray>& ioVertexArray)
{
    tinyobj::ObjReaderConfig reader_config;
    tinyobj::ObjReader reader;

    if(!reader.ParseFromString(inModelFile.raw_data_str(), "", reader_config))
    {
        if(!reader.Error().empty())
            { print_error("TinyObjReader Error: '{}'", reader.Error()); }
        return ERR_FILE_READ;
    }

    if(!reader.Warning().empty())
        { print_warning("TinyObjReader Warning: '{}'", reader.Warning()); }

    auto& attrib{reader.GetAttrib()};
    auto& shapes{reader.GetShapes()};

    uint   vertex_count{0};
    Vertex temp_vertex{};

    std::vector<uint> indices{};
    std::vector<float> vertices{};

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

                temp_vertex.GetVertexData(vertices);
                indices.push_back(vertex_count++);
            }
            index_offset += fv;
        }
    }
    ioVertexArray->SetVertexBuffer(VertexBuffer::Create(vertices.data(), vertices.size()));
    ioVertexArray->SetIndexBuffer(IndexBuffer::Create(indices.data(), indices.size()));
    return OK;
}
