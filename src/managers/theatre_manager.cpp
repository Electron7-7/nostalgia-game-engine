#include "theatre_manager.hpp"
#include "safe_return.hpp"
#include "theatre/theatre_data.hpp"
#include "theatre/theatre_parser.hpp"
#include "rendering/render_command.hpp"
#include "things/things.hpp"
#include "things/actors/nostalgia_player.hpp"
#include "types/typenames.hpp"
#ifdef DEBUGGING
#   include "colors.hpp"
#   include "time.hpp"
#   include "testing_app/ui/imgui_debugger.hpp"
#endif // DEBUGGING

#include <vector>
#include <glm/glm.hpp>

using namespace ManagerEnums;

static std::shared_ptr<Thing> s_LocalPlayer = std::make_shared<NostalgiaPlayer>();

static std::vector<RenderCommand> s_RenderCommandQueue = {};

static TheatreManager s_TheatreManager;
TheatreManager* g_pTheatreManager = &s_TheatreManager;

bool TheatreManager::s_AreThingsLocked    = false;
std::map<id_t, std::shared_ptr<Thing>>    TheatreManager::s_Things    = {};

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

    for(data_t& data : theatre_data)
    {
        auto& thing = s_Things[data.GetID()] = g_MakeThing(data.GetType())();
        thing->m_ID   = data.GetID();
        thing->m_Type = data.GetType();
        thing->m_Name = data.GetName();
        if(data.GetType() == Type::NostalgiaPlayer)
            { s_LocalPlayer = thing; }
        thing->SetupVariables(data);
    }

    s_AreThingsLocked = false;
}

void TheatreManager::DestroyObjects()
{
    WAIT((s_AreThingsLocked), 1.0f)
    s_AreThingsLocked = true;

    s_Things.clear();

    s_AreThingsLocked = false;
}

bool TheatreManager::try_DestroyThing(id_t thing_id)
{
    WAIT(s_AreThingsLocked, 10.0f)

    s_AreThingsLocked = true;

    if(!s_Things.contains(thing_id))
        { return false; }

    s_Things.erase(thing_id);

    s_AreThingsLocked = false;
    return true;
}

#ifdef DEBUGGING
    const std::map<id_t, std::shared_ptr<Thing>>& TheatreManager::debug_GetThings()
    { return s_Things; }
#endif // DEBUGGING
