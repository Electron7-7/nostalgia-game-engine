#include "theatre_manager.hpp"
#ifdef DEBUGGING
#   include "colors.hpp"
#   include "time.hpp"
#endif // DEBUGGING
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

    if(!SafeStatus::PrintCheck(TheatreParser::try_ParseTheatre()))
        { return FUCKED; }

    ResourceHandler::CreateResources();
    // CreateThings();

    return FINISHED;
}

ManagerEnums::TheatreReturnValue_t TheatreManager::TheatreShutdown(bool is_first_call)
{
    if(!is_first_call)
        { return FINISHED; }

    ResourceHandler::DestroyResources();
    // DestroyThings();

    return FINISHED;
}

// TODO: Replace this with a ThingHandler (or just make TheatreManager the "Thing handler")
void TheatreManager::CreateThings()
{}

// TODO: Replace this with a ThingHandler (or just make TheatreManager the "Thing handler")
void TheatreManager::DestroyThings()
{}
