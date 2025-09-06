#include "theatre_manager.hpp"
#ifdef DEBUGGING
#   include "colors.hpp"
#   include "time.hpp"
#   include "testing_app/ui/imgui_debugger.hpp"
#endif // DEBUGGING
#include "things/thing.hpp"
#include "rendering/render_command.hpp"
#include "theatre/theatre_parser.hpp"
#include "resources/resource_handler.hpp"

#include <vector>
#include <glm/glm.hpp>

using namespace ManagerEnums;

static TheatreManager s_TheatreManager;
TheatreManager* g_pTheatreManager = &s_TheatreManager;

static std::vector<RenderCommand> s_RenderCommandQueue = {};
static std::vector<Thing*> s_ThingStorage = {};

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

    ResourceHandler::CreateResources();
    // CreateThings();

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

    ResourceHandler::DestroyResources();
    // DestroyThings();

#ifdef DEBUGGING
    g_pDebugger->StopTheatreTiming(false);
#endif // DEBUGGING

    return FINISHED;
}

// TODO: Replace this with a ThingHandler (or just make TheatreManager the "Thing handler")
void TheatreManager::CreateThings()
{}

// TODO: Replace this with a ThingHandler (or just make TheatreManager the "Thing handler")
void TheatreManager::DestroyThings()
{}
