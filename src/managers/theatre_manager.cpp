#include "theatre_manager.hpp"

#include <glm/glm.hpp>

using namespace ManagerEnums;

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

void TheatreManager::CreateThings()
{}

void TheatreManager::DestroyThings()
{}
