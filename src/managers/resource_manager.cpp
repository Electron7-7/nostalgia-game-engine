#include "resource_manager.hpp"
#include "embedded/images.hpp"
#include "resources/resource.hpp"
#include "resources/mesh.hpp"
#include "resources/texture.hpp"
#include "embedded/models.hpp"
#include "printing.hpp"
#include "time.hpp"
#include "sanity_executable_locator.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_USE_MAPBOX_EARCUT
#include "TinyOBJLoader/tiny_obj_loader.h"

#include <random>
#include <map>

using namespace ManagerEnums;

const rid_t RID::ErrorMesh      = 0x1;
const rid_t RID::RamielMesh     = 0x2;
const rid_t RID::MissingTexture = 0x3;
const rid_t RID::DoomTexture    = 0x4;

static const std::set<rid_t> s_cDoNotRemove =
{
    RID::ErrorMesh,
    RID::RamielMesh,
    RID::MissingTexture,
    RID::DoomTexture
};
static bool s_AreSpecialResourcesInitialized = false;

static std::map<rid_t, Resource*> s_RIDMap = {};

static std::vector<Mesh> s_MeshStorage = {};
static std::vector<Texture> s_TextureStorage = {};
// static std::vector<MultiTexture> s_MultiTextureStorage = {};

static std::random_device s_RandomDevice;

static ResourceManager s_ResourceManager;
ResourceManager* g_pResourceManager = &s_ResourceManager;

void ResourceManager::m_GenerateSpecialResources()
{
    if(s_AreSpecialResourcesInitialized)
    { return; }

    m_try_CreateObjMesh(RID::ErrorMesh, model_OBJ_Error, "Error Mesh");
    m_try_CreateObjMesh(RID::RamielMesh, model_OBJ_Ramiel, "Ramiel Mesh");
    m_AddTexture(RID::MissingTexture, {image_JPG_MISSINGTEXTURE, image_JPG_MISSINGTEXTURE_len}, "Missing Texture");
    m_AddTexture(RID::DoomTexture, {image_PNG_COMP045, image_PNG_COMP045_len}, "Doom Texture");

    s_AreSpecialResourcesInitialized = true;
}

bool ResourceManager::Init()
{
    s_MeshStorage.reserve(2);
    s_TextureStorage.reserve(2);

    m_GenerateSpecialResources();

    return true;
}

void ResourceManager::Shutdown()
{
    m_ClearAllResources();
}

void ResourceManager::ClearResources()
{
    m_ClearAllResources();
    m_GenerateSpecialResources();
}

void ResourceManager::m_ClearAllResources()
{
    m_AreResourcesLocked = true;

    // TODO: Make sure this doesn't cause issues later in development
    s_MeshStorage.clear();
    s_TextureStorage.clear();
    s_RIDMap.clear();

    s_AreSpecialResourcesInitialized = false;
    m_AreResourcesLocked = false;
}

bool ResourceManager::try_RemoveResource(rid_t resource_id)
{
    WAIT_FOR(!m_AreResourcesLocked, 10.0f)

    if(s_cDoNotRemove.contains(resource_id))
    { return false; }

    if(!s_RIDMap.contains(resource_id))
    { return false; }

    s_RIDMap.erase(resource_id);
    return true;
}

bool ResourceManager::try_SlowRemoveResource(Resource* resource_pointer)
{
    for(auto& [id, resource] : s_RIDMap)
    {
        WAIT_FOR(!m_AreResourcesLocked, 10.0f)

        if(resource != resource_pointer)
        { continue; }

        if(s_cDoNotRemove.contains(id))
        { return false; }

        s_RIDMap.erase(id);
        return true;
    }
    return false;
}

SafeReturn<Resource*> try_FindResource(rid_t resource_id)
{
    if(!s_RIDMap.contains(resource_id))
    { return SafeReturn<Resource*>(nullptr, Status::ResourceManagerINVALID_RESOURCE_ID); }

    return SafeReturn(s_RIDMap.at(resource_id));
}

rid_t ResourceManager::m_GetNewResourceID()
{
    rid_t return_value = 1;

    std::mt19937 engine(s_RandomDevice());
    std::uniform_int_distribution<rid_t> distribution(1);

    while(s_RIDMap.contains(return_value))
    { return_value = distribution(engine); }

    return return_value;
}

rid_t ResourceManager::CreateObjMesh(const std::string& model_data, const std::string& mesh_name)
{
    rid_t mesh_rid = m_GetNewResourceID();
    if(!SafeStatus::PrintCheck(m_try_CreateObjMesh(mesh_rid, model_data, mesh_name)))
    { mesh_rid = RID::ErrorMesh; }
    return mesh_rid;
}

rid_t ResourceManager::CreateTexture(const TextureData& image_data, const std::string& texture_name)
{
    rid_t texture_id = m_GetNewResourceID();
    m_AddTexture(texture_id, image_data, texture_name);
    return texture_id;
}

rid_t ResourceManager::CreateTexture(const std::string& image_file, const std::string& texture_name)
{
    rid_t texture_rid = m_GetNewResourceID();
    if(!SafeStatus::PrintCheck(m_try_CreateTexture(texture_rid, image_file, texture_name)))
    { texture_rid = RID::MissingTexture; }
    return texture_rid;
}

// Taken from GraphX
SafeStatus ResourceManager::m_try_CreateObjMesh(rid_t resource_id, const std::string& model_data, const std::string& mesh_name)
{
    tinyobj::ObjReaderConfig reader_config;
    tinyobj::ObjReader reader;

    if(!reader.ParseFromString(model_data, "", reader_config))
    {
        if(!reader.Error().empty())
        {
            PRINT_ERROR("TinyObjReader Error: '{}'", reader.Error())

            return Status::ResourceManagerFAILED_TO_LOAD_OBJ;
        }
    }

    if(!reader.Warning().empty())
    { PRINT_WARNING("TinyObjReader Warning: '{}'", reader.Warning()) }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();

    m_AreResourcesLocked = true;

    // FIXME: Is doing this and accessing the referenced variable safe?
    // the justification is to disable `m_AreResourcesLocked` as fast as possible...
    Mesh& new_mesh = s_MeshStorage.emplace_back();
    s_RIDMap[resource_id] = &s_MeshStorage.back();
    s_MeshStorage.back().m_Name = mesh_name;

    m_AreResourcesLocked = false;

    Vertex temp_vertex;

    // Loop over shapes
    // Shapes are full meshes in the OBJ (there can be multiple)
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

    // new_mesh.fixOBJData();
    return Status::NO_ERROR;
}

SafeStatus ResourceManager::m_try_CreateTexture(rid_t texture_id, const std::string& file_path, const std::string& texture_name)
{
    std::string binary_path = BINARY_PATH;

    // If the file path is relative, it should be relative to the program's location
    std::string file_path_checked = std::string(binary_path + file_path);

    // If the file path is absolute, use it as is
    if(file_path.starts_with('/') || !file_path.substr(1, 2).compare(":/"))
    { file_path_checked = file_path; }

    // The rest of this code is thanks to: https://stackoverflow.com/a/22131201
    FILE* image_file = fopen(file_path_checked.c_str(), "r+");

    if(image_file == nullptr)
    {
        PRINT_ERROR("ResourceManager::m_try_CreateTexture - Image file unable to be read / does not exist!")
        return Status::ResourceManagerFAILED_TO_LOAD_IMAGE;
    }

    fseek(image_file, 0, SEEK_END);
    unsigned int image_size = ftell(image_file); // This could overflow... too bad!
    fclose(image_file);

    image_file = fopen(file_path_checked.c_str(), "r+"); // FIXME: Figure out if I need to close and re-open the file
    unsigned char* file_data = new unsigned char;
    fread(file_data, sizeof(unsigned char), image_size, image_file);
    fclose(image_file);

    m_AreResourcesLocked = true;

    s_TextureStorage.emplace_back(TextureData{file_data, image_size});
    s_RIDMap[texture_id] = &s_TextureStorage.back();
    s_TextureStorage.back().m_Name = texture_name;

    m_AreResourcesLocked = false;
    return Status::NO_ERROR;
}

void ResourceManager::m_AddTexture(rid_t resource_id, const TextureData& texture_data, const std::string& texture_name)
{
    m_AreResourcesLocked = true;

    s_TextureStorage.emplace_back(texture_data);
    s_RIDMap[resource_id] = &s_TextureStorage.back();
    s_TextureStorage.back().m_Name = texture_name;

    m_AreResourcesLocked = false;
}
