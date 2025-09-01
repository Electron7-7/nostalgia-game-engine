#include "theatre_manager.hpp"
#include "rendering/render_command.hpp"
#include "theatre/theatre_parser.hpp"

#include <vector>
#include <glm/glm.hpp>

using namespace ManagerEnums;

static TheatreManager s_TheatreManager;
TheatreManager* g_pTheatreManager = &s_TheatreManager;

static std::vector<RenderCommand> s_RenderCommandQueue = {};
static std::vector<Thing*> s_ThingStorage = {};

ManagerEnums::TheatreReturnValue_t TheatreManager::TheatreInit(bool is_first_call)
{
    if(!is_first_call)
    { return FINISHED; }

    if(!SafeStatus::PrintCheck(TheatreParser::try_ParseTheatre()))
        { return FUCKED; }

    CreateThings();

    return FINISHED;
}

ManagerEnums::TheatreReturnValue_t TheatreManager::TheatreShutdown(bool is_first_call)
{
    if(!is_first_call)
    { return FINISHED; }

    DestroyThings();

    return FINISHED;
}

void TheatreManager::CreateThings()
{}

void TheatreManager::DestroyThings()
{}
