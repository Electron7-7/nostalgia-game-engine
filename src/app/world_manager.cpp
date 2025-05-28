#include "world_manager.hpp"
#include "render_manager.hpp"

static WorldManager singleton_WorldManager;
WorldManager* global_WorldManager = &singleton_WorldManager;

// Console variables for setting the forward and backward speed of the camera (hl2_src/app/legion/worldmanager.cpp:24-25)

WorldManager::WorldManager()
{
    // world_HeightManager = nullptr;
}

WorldManager::~WorldManager()
{
    // assert(world_HeightManager == nullptr);
}

TheatreReturnValue_t WorldManager::TheatreInit(bool is_first_call)
{
    if(!is_first_call)
        return FINISHED;

    // assert(!world_HeightField);
    // world_HeightField = new HeightField(6, 6, 4);
    // if(!world_HeightField->LoadHeightFromFile("maps/testheight.psd"))
    //     return FAILED;

    CreateThings();
    SetInitialLocalPlayerPosition();
    return FINISHED;
}

TheatreReturnValue_t WorldManager::TheatreShutdown(bool is_first_call)
{
    if(!is_first_call)
        return FINISHED;

    DestroyThings();

    // if(world_HeightField)
    // {
    //  delete world_HeightField;
    //  world_HeightField = nullptr;
    // }
    return FINISHED;
}

void WorldManager::CreateThings()
{ world_Player.player_CameraProperty = global_RenderManager->CreateCameraProperty(); }

void WorldManager::DestroyThings()
{ global_RenderManager->DestroyCameraProperty(world_Player.player_CameraProperty); }

NostalgiaPlayerActor* WorldManager::GetLocalPlayer()
{ return &world_Player; }

void WorldManager::SetInitialLocalPlayerPosition()
{
    float distance = 1024.0;
    glm::vec3 camera_direction(1.0f, 1.0f, -0.5f);
    camera_direction = glm::normalize(camera_direction);

    world_Player.player_CameraProperty->origin.x = 512 + camera_direction.x * distance;
    world_Player.player_CameraProperty->origin.y = 0   + camera_direction.y * distance;
    world_Player.player_CameraProperty->origin.z = 512 + camera_direction.z * distance;
}

void WorldManager::DrawWorld()
{
    // world_HeightField->Draw();
}

// The rest are the console command functions (see hl2_src/app/legion/worldmanager.cpp:128-166)