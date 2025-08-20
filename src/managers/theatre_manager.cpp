#include "theatre_manager.hpp"
#include "rendering/camera_property.hpp"

#include <glm/glm.hpp>

using namespace ManagerEnums;

TheatreManager singleton_TheatreManager;
TheatreManager* global_TheatreManager = &singleton_TheatreManager;

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
    SetInitialLocalPlayerPosition();

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

NostalgiaPlayer* TheatreManager::GetLocalPlayer()
{ return &world_Player; }

void TheatreManager::SetInitialLocalPlayerPosition()
{
    // FIXME: This is hardcoded and bad; don't do this
    float distance = 1024.0;
    glm::vec3 camera_direction(1.0f, 1.0f, -0.5f);
    camera_direction = glm::normalize(camera_direction);

    world_Player.GetCameraProperty()->origin.x = 512 + camera_direction.x * distance;
    world_Player.GetCameraProperty()->origin.y = 0   + camera_direction.y * distance;
    world_Player.GetCameraProperty()->origin.z = 512 + camera_direction.z * distance;
}

void TheatreManager::RenderWorld()
{
    // for(int i = 0 ; i < world_RenderCommandQueue.size() ; i++)
    // {
        // global_BackendManager->GetGraphicsBackend()->Draw3D(world_RenderCommandQueue.at(i));
        // world_RenderCommandQueue.erase(world_RenderCommandQueue.begin() + i);
    // }
}
// The rest are the console command functions (see hl2_src/app/legion/worldmanager.cpp:128-166)
