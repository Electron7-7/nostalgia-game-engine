#include "./mesh.hpp"
#include "things/thing_factory.hpp"
#include "rendering/mesh_buffers.hpp"
#include "rendering/vertex.hpp"
#include "rendering/vertex_array.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_USE_MAPBOX_EARCUT
#include "thirdparty/TinyOBJLoader/tiny_obj_loader.h"

using namespace TheatreFile;

static Error s_CreateMeshData(std::vector<float>&, std::vector<uint>&, Farg<Shared<FileData>>);
static Error s_CreateOBJMesh(std::vector<float>&, std::vector<uint>&, Farg<Shared<FileData>>);

Shared<Mesh> Mesh::CreateFromMemory(const uchar* inData, size_t inSize, ModelType inType)
{
    Shared<Mesh> output{DCast<Mesh>(ThingFactory::MakeThing(ThingType::Mesh, "Untitled_Mesh"))};
    switch(inType)
    {
    case MODEL_OBJ:
        output->m_pModel = MakeShared<FileData>(inData, inSize, FileType::model_OBJ);
        break;
    default:
        print_error("Invalid model type");
        output->m_pModel = MakeShared<FileData>();
        break;
    }
    output->LoadModel();
    return output;
}

void Mesh::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);

    data.get_variable(mMaterialID, "Material");
    if(data.get_variable(m_pModel, "Model", "File") == OK)
        { LoadModel(); }
}

Shared<ThingData> Mesh::GetVariables() const
{
    auto data{Super::GetVariables()};

    data->set_variable(m_pModel, "Model");
    data->set_variable(mMaterialID, "Material");

    return data;
}

Farg<Shared<VertexArray>> Mesh::MeshData() const
{ return mVertexArray; }

void Mesh::MeshData(Shared<VertexArray> inMeshData)
{ mVertexArray = inMeshData; }

ID Mesh::MaterialID() const
{ return mMaterialID; }

void Mesh::MaterialID(ID inID)
{ mMaterialID = inID; }

Error Mesh::LoadModel()
{
    mVertexArray = VertexArray::Create();
    std::vector<float> vertices{};
    std::vector<uint>  indices{};
    if(Error status{s_CreateMeshData(vertices, indices, m_pModel)}; not status)
    {
        print_error("Mesh [{}, {}] failed to parse mesh data", mName, uid()());
        return status;
    }
    mVertexArray->AddVertexBuffer(VertexBuffer::Create(vertices.data(), vertices.size()));
    mVertexArray->SetIndexBuffer(IndexBuffer::Create(indices.data(), indices.size()));
    return OK;
}

//////////////////
// MESH PARSING //
//////////////////
Error s_CreateMeshData(std::vector<float>& ioVerts, std::vector<uint>& ioInds, Farg<Shared<FileData>> inData)
{
    if(!inData->Status())
        { return inData->Status(); }

    switch(inData->Type())
    {
    case FileType::model_OBJ:
        return s_CreateOBJMesh(ioVerts, ioInds, inData);
    case FileType::Unknown:
    default:
        print_error("Data of unknown file-type cannot be buffered");
        return ERR_INVALID_TYPE;
    }
    return OK;
}

Error s_CreateOBJMesh(std::vector<float>& ioVerts, std::vector<uint>& ioInds, Farg<Shared<FileData>> inData)
{
    tinyobj::ObjReaderConfig reader_config;
    tinyobj::ObjReader reader;

    if(!reader.ParseFromString(inData->DataString(), "", reader_config))
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

                temp_vertex.GetVertexData(ioVerts);
                ioInds.push_back(vertex_count++);
            }
            index_offset += fv;
        }
    }
    return OK;
}
