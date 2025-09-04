#include "resource_handler.hpp"
#include "basic/mesh.hpp"
#include "basic/texture.hpp"
#include "theatre/theatre_parser.hpp"
#include "time.hpp"
#include "types/typenames.hpp"

#include <random>
#include <map>

static bool s_AreResourcesLocked = false;

static std::map<rid_t, Resource*> s_Resources = {};

static std::map<rid_t, Mesh>    s_Meshes   = {};
static std::map<rid_t, Texture> s_Textures = {};

static std::random_device s_RandomDevice;

void ResourceHandler::CreateResources()
{
    std::vector<data_t> data = TheatreParser::GetTheatreData().GetResources();
    std::map<std::string, rid_t> l_NameIDMap = {};

    WAIT(s_AreResourcesLocked, 1)
    s_AreResourcesLocked = true;

    for(data_t& resource_data : data)
    {
        rid_t id = GetNewResourceID();

        switch(resource_data.GetType())
        {
        case Type::Mesh:
            s_Meshes[id]    = Mesh();
            s_Resources[id] = &s_Meshes.at(id);
            break;
        case Type::Texture:
            s_Textures[id]  = Texture();
            s_Resources[id] = &s_Textures.at(id);
            break;
        default:
            PRINT_WARNING("ResourceHandler::CreateResources - Attempted to process a 'data_t' object with an invalid/unhandled type: '{}'. The object '{}' will not be used", resource_data.GetTypeName(), resource_data.GetName())
            id = Resource::NoRID;
            break;
        }

        resource_data.m_AssignedID = id;
        l_NameIDMap[resource_data.GetName()] = id;
    }

    for(data_t& resource_data : data)
    {
        resource_data.UpdateTheatreReferences(l_NameIDMap);
        SetVariables(resource_data.m_Type, resource_data.m_AssignedID, resource_data);
    }

    s_AreResourcesLocked = false;
}

void ResourceHandler::DestroyResources()
{
    WAIT(s_AreResourcesLocked, 1.0f)

    s_AreResourcesLocked = true;

    s_Resources.clear();

    s_AreResourcesLocked = false;
}

bool ResourceHandler::try_DestroyResource(rid_t resource_id)
{
    WAIT(s_AreResourcesLocked, 10.0f)

    s_AreResourcesLocked = true;

    if(!s_Resources.contains(resource_id))
        { return false; }

    s_Resources.erase(resource_id);
    delete s_Resources.at(resource_id);

    s_AreResourcesLocked = false;
    return true;
}

SafeReturn<Resource*> ResourceHandler::try_GetResource(rid_t resource_id)
{
    if(!s_Resources.contains(resource_id))
        { return SafeReturn(&Resource::Empty, Status::ResourceHandlerINVALID_RESOURCE_ID); }

    return SafeReturn(s_Resources.at(resource_id));
}

rid_t ResourceHandler::GetNewResourceID()
{
    rid_t return_value = 1;

    std::mt19937 engine(s_RandomDevice());
    std::uniform_int_distribution<rid_t> distribution(1);

    while(s_Resources.contains(return_value))
        { return_value = distribution(engine); }

    return return_value;
}

void ResourceHandler::SetVariables(size_t type, rid_t id, const data_t& data)
{
    if(!s_Resources.contains(id))
        { return; }

    switch(type)
    {
    case Type::Invalid:
        // Do something...
        break;
    case Type::Resource:
        {
            Resource* resource = s_Resources.at(id);

            resource->m_ID   = id;
            resource->m_Name = data.GetName();
            break;
        }
    case Type::Mesh:
        {
            SetVariables(Type::Resource, id, data);

            Mesh& mesh = s_Meshes.at(id);

            data.GetString(mesh.m_MeshFile, "File");
            data.GetEngineRef(mesh.m_MeshFileData, "Data");
            break;
        }
    case Type::Texture:
        {
            SetVariables(Type::Resource, id, data);

            Texture& texture = s_Textures.at(id);

            data.GetString(texture.m_File, "File");
            data.GetEngineRef(texture.m_Data, "Data");
        }
    }
}
