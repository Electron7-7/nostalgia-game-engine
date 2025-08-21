#include "resource_manager.hpp"
#include "embedded/meshes.hpp"
#include "printing.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_USE_MAPBOX_EARCUT
#include "TinyOBJLoader/tiny_obj_loader.h"

#include <map>

using namespace ManagerEnums;

static std::map<ResourceID, Mesh> g_MeshStorage = {};

static ResourceManager s_ResourceManager;
ResourceManager* g_pResourceManager = &s_ResourceManager;

bool ResourceManager::Init()
{
    RID::meshCube = LoadGraphXMesh(&mesh_GRAPHX_Cube);
    RID::meshQuad = LoadGraphXMesh(&mesh_GRAPHX_Quad);
    RID::meshPyramid = LoadGraphXMesh(&mesh_GRAPHX_Pyramid);
    // Unsafe loading of embedded OBJ files, because I trust myself to embed uncorrupted files (this will come back to haunt me)
    RID::meshRamiel = try_LoadOBJMeshFromMemory(mesh_OBJ_Ramiel).Data();

    return true;
}

void ResourceManager::Shutdown()
{
    // TODO: Make sure this won't cause crahes later on in development
    g_MeshStorage.clear();

    RID::meshCube = ResourceID();
    RID::meshQuad = ResourceID();
    RID::meshPyramid = ResourceID();
    RID::meshRamiel = ResourceID();
}

TheatreReturnValue_t ResourceManager::TheatreInit(bool IsFirstCall)
{ return FINISHED; }

TheatreReturnValue_t ResourceManager::TheatreShutdown(bool IsFirstCall)
{ return FINISHED; }

ResourceID ResourceManager::LoadGraphXMesh(Mesh* graphx_mesh, const std::string& mesh_name)
{
    ResourceID return_value(mesh_name, g_MeshStorage.size());
    g_MeshStorage[return_value] = *graphx_mesh;
    return return_value;
}

SafeReturn<ResourceID> ResourceManager::try_LoadOBJMeshFromMemory(const std::string& embedded_obj_file, const std::string& mesh_name)
{
    ResourceID return_value(mesh_name, g_MeshStorage.size());

    Mesh& new_mesh = g_MeshStorage[return_value] = Mesh();

    tinyobj::ObjReaderConfig reader_config;
    tinyobj::ObjReader reader;

    if(!reader.ParseFromString(embedded_obj_file, "", reader_config))
    {
        if(!reader.Error().empty())
        {
            PRINT_ERROR("TinyObjReader Error: '{}'", reader.Error())

            g_MeshStorage.erase(return_value);
            return SafeReturn(return_value, Status::ResourceManagerFAILED_TO_LOAD_OBJ);
        }
    }

    if(!reader.Warning().empty())
    { PRINT_WARNING("TinyObjReader Warning: '{}'", reader.Warning()) }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();

    Vertex temp_vertex;

    // Loop over shapes
    // Shapes are full meshes in the OBJ
    for (size_t s = 0; s < shapes.size(); s++)
    {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++)
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

                new_mesh.AddVertex(temp_vertex);
            }

            index_offset += fv;
        }
    }

    // new_mesh.fixOBJData(); // From GraphX
    return SafeReturn(return_value);
}
