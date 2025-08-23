#include "theatre_manager.hpp"
#include "rendering/render_command.hpp"
#include "theatre/theatre_interpreting.hpp"

#include <vector>
#include <glm/glm.hpp>

using namespace ManagerEnums;

static std::vector<RenderCommand> s_RenderCommandQueue = {};
static std::vector<Thing*> s_ThingStorage = {};

static TheatreManager s_TheatreManager;
TheatreManager* g_pTheatreManager = &s_TheatreManager;

// Console variables for setting the forward and backward speed of the camera (hl2_src/app/legion/worldmanager.cpp:24-25)

TheatreManager::TheatreManager()
{
}

TheatreManager::~TheatreManager()
{
}

ManagerEnums::TheatreReturnValue_t TheatreManager::TheatreInit(bool is_first_call)
{
    if(!is_first_call)
    { return FINISHED; }

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

void TheatreManager::LoadTheatreFromMemory(const Theatre& new_theatre)
{ m_CurrentTheatre = new_theatre; }

bool TheatreManager::try_LoadTheatre(const std::string& theatre_file)
{
    if(SafeStatus::PrintCheck(try_LoadTheatreFile(theatre_file)))
    {
        m_CurrentTheatre = InterpretTheatreFile();
        return true;
    }

    return false;
}

void TheatreManager::CreateThings()
{}

void TheatreManager::DestroyThings()
{}
