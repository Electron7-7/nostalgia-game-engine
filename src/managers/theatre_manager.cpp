#include "theatre_manager.hpp"
#include "safe_return.hpp"
#include "time.hpp"
#include "theatre/theatre_parser.hpp"
#include "types/typenames.hpp"
#include "rendering/render_command.hpp"
#include "things/thinker.hpp"
#include "things/actor.hpp"
#include "things/actors/prototype_actor.hpp"
#include "things/actors/nostalgia_player.hpp"
#include "things/actors/camera_3d.hpp"
#include "resources/resource.hpp"
#include "resources/basic/mesh.hpp"
#include "resources/basic/texture.hpp"
#include "resources/complex/material.hpp"
#include "resources/complex/mesh_instance.hpp"
#ifdef DEBUGGING
#   include "colors.hpp"
#   include "time.hpp"
#   include "testing_app/ui/imgui_debugger.hpp"
#endif // DEBUGGING

#include <vector>
#include <random>
#include <glm/glm.hpp>

using namespace ManagerEnums;

static constexpr const char* s_TypeWarning  = "{} TheatreManager::CreateObjects - '{}' has an {} and cannot be interpreted{}";

static std::shared_ptr<NostalgiaPlayer> s_LocalPlayer = std::make_shared<NostalgiaPlayer>();
static std::vector<RenderCommand> s_RenderCommandQueue = {};

static TheatreManager s_TheatreManager;
TheatreManager* g_pTheatreManager = &s_TheatreManager;

bool TheatreManager::s_AreResourcesLocked = false;
bool TheatreManager::s_AreThingsLocked    = false;
std::map<id_t, std::shared_ptr<Thing>>    TheatreManager::s_Things    = {};
std::map<id_t, std::shared_ptr<Resource>> TheatreManager::s_Resources = {};

void TheatreManager::Update()
{
#ifdef DEBUGGING
    if(g_PrintFrameNumbers)
        { std::print("{}TheatreManager::Update #{} @ {}\n", sty::Bold + fg::Green, Manager::m_sFrameNumber, Time::Elapsed()); }
#endif // DEBUGGING
}

void TheatreManager::Tick()
{
#ifdef DEBUGGING
    if(g_PrintTickNumbers)
        { std::print("{}TheatreManager::Tick #{} @ {}\n", sty::Bold + fg::Blue, Manager::m_sTickNumber, Time::Elapsed()); }
#endif // DEBUGGING
}

ManagerEnums::TheatreReturnValue_t TheatreManager::TheatreInit(bool is_first_call)
{
    if(!is_first_call)
        { return FINISHED; }

#ifdef DEBUGGING
    g_pDebugger->StartTheatreTiming(true);
#endif // DEBUGGING

    if(!SafeStatus::PrintCheck(TheatreParser::try_ParseTheatre()))
        { return FUCKED; }

    CreateObjects();

#ifdef DEBUGGING
    g_pDebugger->StopTheatreTiming(true);
#endif // DEBUGGING

    return FINISHED;
}

ManagerEnums::TheatreReturnValue_t TheatreManager::TheatreShutdown(bool is_first_call)
{
    if(!is_first_call)
        { return FINISHED; }

#ifdef DEBUGGING
    g_pDebugger->StartTheatreTiming(false);
#endif // DEBUGGING

    DestroyObjects();

#ifdef DEBUGGING
    g_pDebugger->StopTheatreTiming(false);
#endif // DEBUGGING

    return FINISHED;
}

void TheatreManager::CreateObjects()
{
    std::vector<data_t> theatre_data = TheatreParser::GetTheatreData().GetData();

    WAIT(s_AreThingsLocked, 1.0f)
    s_AreThingsLocked = true;
    s_AreResourcesLocked = true;

    for(data_t& data : theatre_data)
    {
        tid_t id = GetNewID();

        switch(data.GetType())
        {
        case Type::Resource:
            s_Resources[id] = std::make_shared<Resource>();
            break;
        case Type::Mesh:
            s_Resources[id] = std::make_shared<Mesh>();
            break;
        case Type::Texture:
            s_Resources[id] = std::make_shared<Texture>();
            break;
        case Type::Material:
            s_Resources[id] = std::make_shared<Material>();
            break;
        case Type::MeshInstance:
            s_Resources[id] = std::make_shared<MeshInstance>();
            break;
        case Type::Thing:
            s_Things[id] = std::make_shared<Thing>();
            break;
        case Type::Thinker:
            s_Things[id] = std::make_shared<Thinker>();
            break;
        case Type::Actor:
            s_Things[id] = std::make_shared<Actor>();
            break;
        case Type::PrototypeActor:
            s_Things[id] = std::make_shared<PrototypeActor>();
            break;
        case Type::NostalgiaPlayer:
            s_LocalPlayer = std::make_shared<NostalgiaPlayer>();
            s_Things[id]  = s_LocalPlayer;
            break;
        case Type::Invalid:
            std::print(s_TypeWarning, __WARNING, data.m_Name, "invalid type", __RESET_NL);
            id = Resource::NoRID;
            break;
        default:
            std::print(s_TypeWarning, __WARNING, data.m_Name, "unknown type (" + data.GetTypeName() + ")", __RESET_NL);
            id = Resource::NoRID;
            break;
        }

        data.m_AssignedID = id;
        l_NameIDMap[data.m_Name] = id;
    }

    s_AreResourcesLocked = false;
    s_AreThingsLocked = false;

    for(data_t& data : theatre_data)
    {
        data.UpdateTheatreReferences(l_NameIDMap);
        SetVariables(data.m_Type, data.m_AssignedID, data);
    }
}

void TheatreManager::DestroyObjects()
{
    WAIT((s_AreThingsLocked || s_AreResourcesLocked), 1.0f)
    s_AreThingsLocked = true;
    s_AreResourcesLocked = true;

    s_Things.clear();
    s_Resources.clear();

    s_AreResourcesLocked = false;
    s_AreThingsLocked = false;
}

/*void TheatreManager::CreateResources()
{
    std::vector<data_t> data = TheatreParser::GetTheatreData().GetResources();
    std::map<std::string, rid_t> l_NameIDMap = {};

    WAIT(s_AreResourcesLocked, 1)
    s_AreResourcesLocked = true;

    for(data_t& resource_data : data)
    {
        rid_t id = GetNewID();

        switch(resource_data.GetType())
        {
        case Type::Mesh:
            s_Resources[id] = std::make_shared<Mesh>();
            break;
        case Type::Texture:
            s_Resources[id] = std::make_shared<Texture>();
            break;
        case Type::Material:
            s_Resources[id] = std::make_shared<Material>();
            break;
        case Type::MeshInstance:
            s_Resources[id] = std::make_shared<MeshInstance>();
            break;
        case Type::Invalid:
            // PRINT_WARNING
            std::print(s_TypeWarning, __WARNING, s_ResourcesStr, resource_data.GetName(), "invalid type", __RESET_NL);
        default:
            // PRINT_WARNING
            std::print(s_TypeWarning, __WARNING, s_ResourcesStr, resource_data.GetName(), "unknown type (" + resource_data.GetTypeName() + ")", __RESET_NL);
            id = Resource::NoRID;
            break;
        }

        resource_data.m_AssignedID = id;
        l_NameIDMap[resource_data.GetName()] = id;
    }

    s_AreResourcesLocked = false;

    for(data_t& resource_data : data)
    {
        resource_data.UpdateTheatreReferences(l_NameIDMap);
        SetVariables(resource_data.m_Type, resource_data.m_AssignedID, resource_data);
    }
}*/

/*void TheatreManager::DestroyResources()
{
    WAIT(s_AreResourcesLocked, 1.0f)

    s_AreResourcesLocked = true;

    s_Resources.clear();

    s_AreResourcesLocked = false;
}*/

bool TheatreManager::try_DestroyResource(rid_t resource_id)
{
    WAIT(s_AreResourcesLocked, 10.0f)

    s_AreResourcesLocked = true;

    if(!s_Resources.contains(resource_id))
        { return false; }

    s_AreResourcesLocked = false;
    return true;
}

bool TheatreManager::try_DestroyThing(tid_t thing_id)
{
    WAIT(s_AreThingsLocked, 10.0f)

    s_AreThingsLocked = true;

    if(!s_Resources.contains(thing_id))
        { return false; }

    s_Things.erase(thing_id);

    s_AreThingsLocked = false;
    return true;
}

rid_t TheatreManager::GetNewID()
{
    rid_t return_value = 1;

    std::mt19937 engine(s_RandomDevice());
    std::uniform_int_distribution<rid_t> distribution(1);

    while(s_Resources.contains(return_value) || s_Things.contains(return_value))
        { return_value = distribution(engine); }

    return return_value;
}

void TheatreManager::SetVariables(size_t type, rid_t id, const data_t& data)
{
    if(!s_Resources.contains(id) && !s_Things.contains(id))
        { return; }

    switch(type)
    {
    case Type::Invalid:
        // Do Something...
        break;
    case Type::Thing:
        {
            Thing* thing = s_Things.at(id).get();

            thing->m_ID   = id;
            thing->m_Name = data.GetName();
            thing->m_Type = data.GetType();
            break;
        }
    case Type::Thinker:
        {
            SetVariables(Type::Thing, id, data);
            // Do Something...
            break;
        }
    case Type::Actor:
        {
            SetVariables(Type::Thing, id, data);

            Actor* thing = std::dynamic_pointer_cast<Actor>(s_Things.at(id)).get();

            data.GetNumber(thing->m_Origin, "Origin");
            data.GetNumber(thing->m_Rotation, "Rotation");
            break;
        }
    case Type::NostalgiaPlayer:
        {
            SetVariables(Type::Actor, id, data);

            NostalgiaPlayer* thing = std::dynamic_pointer_cast<NostalgiaPlayer>(s_Things.at(id)).get();

            thing->GetCamera()->m_Origin   = thing->m_Origin;
            thing->GetCamera()->m_Rotation = thing->m_Rotation;
            break;
        }
    case Type::PrototypeActor:
        {
            SetVariables(Type::Actor, id, data);

            PrototypeActor* thing = std::dynamic_pointer_cast<PrototypeActor>(s_Things.at(id)).get();

            data.GetTheatreRef(thing->m_MeshInstanceID, "MeshInstance");
            break;
        }
    case Type::Resource:
        {
            Resource* resource = s_Resources.at(id).get();

            resource->m_ID   = id;
            resource->m_Name = data.GetName();
            resource->m_Type = data.GetType();
            break;
        }
    case Type::Mesh:
        {
            SetVariables(Type::Resource, id, data);

            auto resource = GetResource<Mesh>(id);

            data.GetString(resource->m_MeshFile, "File");
            data.GetEngineRef(resource->m_MeshFileData, "Data");
            break;
        }
    case Type::Texture:
        {
            SetVariables(Type::Resource, id, data);

            auto resource = GetResource<Texture>(id);

            data.GetString(resource->m_File, "File");
            data.GetEngineRef(resource->m_Data, "Data");
            break;
        }
    case Type::Material:
        {
            SetVariables(Type::Resource, id, data);

            auto resource = GetResource<Material>(id);

            data.GetTheatreRef(resource->m_DiffuseTextureID, "DiffuseTexture");
            data.GetTheatreRef(resource->m_SpecularTextureID, "SpecularTexture");
            break;
        }
    case Type::MeshInstance:
        {
            SetVariables(Type::Resource, id, data);

            auto resource = GetResource<MeshInstance>(id);

            data.GetTheatreRef(resource->m_MeshID, "Mesh");
            data.GetTheatreRef(resource->m_MaterialID, "Material");
            break;
        }
    default:
        return;
    }
}


#ifdef DEBUGGING
    const std::map<id_t, std::shared_ptr<Thing>>& TheatreManager::debug_GetThings()
    { return s_Things; }

    const std::map<id_t, std::shared_ptr<Resource>>& TheatreManager::debug_GetResources()
    { return s_Resources; }
#endif // DEBUGGING
