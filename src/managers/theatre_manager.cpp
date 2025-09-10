#include "theatre_manager.hpp"
#include "safe_return.hpp"
#include "theatre/theatre_data.hpp"
#include "theatre/theatre_parser.hpp"
#include "rendering/render_command.hpp"
#include "things/things.hpp"
#include "resources/resources.hpp"
#include "types/typenames.hpp"
#ifdef DEBUGGING
#   include "colors.hpp"
#   include "time.hpp"
#   include "testing_app/ui/imgui_debugger.hpp"
#endif // DEBUGGING

#include <vector>
#include <glm/glm.hpp>

using namespace ManagerEnums;

static std::shared_ptr<Thing> s_LocalPlayer = g_MakeThing(Type::NostalgiaPlayer)();

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
        switch(GetBaseType(data.GetType()))
        {
        case Type::Resource:
            s_Resources[data.GetID()] = g_MakeResource(data.GetType())();
            s_Resources.at(data.GetID())->m_ID   = data.GetID();
            s_Resources.at(data.GetID())->m_Type = data.GetType();
            s_Resources.at(data.GetID())->m_Name = data.GetName();
            break;
        case Type::Thing:
            s_Things[data.GetID()] = g_MakeThing(data.GetType())();
            s_Things.at(data.GetID())->m_ID   = data.GetID();
            s_Things.at(data.GetID())->m_Type = data.GetType();
            s_Things.at(data.GetID())->m_Name = data.GetName();
            if(data.GetType() == Type::NostalgiaPlayer)
                { s_LocalPlayer = s_Things.at(data.GetID()); }
            break;
        case Type::Invalid:
            [[fallthrough]];
        default:
            PRINT_WARNING("TheatreManager::CreateObjects - '{}' is an invalid/unknown type ({}) and cannot be interpreted", data.GetName(), data.GetTypeName());
            continue;
        }
    }

    // FIXME: Consolidate/combine these two loops (probably by making 'Thing' and 'Resource' derive the same base class)

    for(data_t& data : theatre_data)
    {
        switch(GetBaseType(data.GetType()))
        {
        case Type::Resource:
            s_Resources.at(data.GetID())->SetupVariables(data);
            continue;
        case Type::Thing:
            s_Things.at(data.GetID())->SetupVariables(data);
            continue;
        default:
            continue;
        }
    }

    s_AreResourcesLocked = false;
    s_AreThingsLocked = false;
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

bool TheatreManager::try_DestroyResource(id_t resource_id)
{
    WAIT(s_AreResourcesLocked, 10.0f)

    s_AreResourcesLocked = true;

    if(!s_Resources.contains(resource_id))
        { return false; }

    s_AreResourcesLocked = false;
    return true;
}

bool TheatreManager::try_DestroyThing(id_t thing_id)
{
    WAIT(s_AreThingsLocked, 10.0f)

    s_AreThingsLocked = true;

    if(!s_Resources.contains(thing_id))
        { return false; }

    s_Things.erase(thing_id);

    s_AreThingsLocked = false;
    return true;
}

#ifdef DEBUGGING
    const std::map<id_t, std::shared_ptr<Thing>>& TheatreManager::debug_GetThings()
    { return s_Things; }

    const std::map<id_t, std::shared_ptr<Resource>>& TheatreManager::debug_GetResources()
    { return s_Resources; }
#endif // DEBUGGING
