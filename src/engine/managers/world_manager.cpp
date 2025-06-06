#include "world_manager.hpp"
#include "backend_manager.hpp"
#include "engine/backends/backend.hpp"
#include "engine/rendering/camera_property.hpp"
#include <glm/glm.hpp>

WorldManager singleton_WorldManager;
WorldManager* global_WorldManager = &singleton_WorldManager;

// Console variables for setting the forward and backward speed of the camera (hl2_src/app/legion/worldmanager.cpp:24-25)

WorldManager::WorldManager()
{
}

WorldManager::~WorldManager()
{
}

TheatreReturnValue_t WorldManager::TheatreInit(bool is_first_call)
{
    if(!is_first_call)
        return FINISHED;

    CreateThings();
    SetInitialLocalPlayerPosition();
    return FINISHED;
}

TheatreReturnValue_t WorldManager::TheatreShutdown(bool is_first_call)
{
    if(!is_first_call)
        return FINISHED;

    DestroyThings();

    return FINISHED;
}

void WorldManager::CreateThings()
{}

void WorldManager::DestroyThings()
{}

NostalgiaPlayerActor* WorldManager::GetLocalPlayer()
{ return &world_Player; }

void WorldManager::SetInitialLocalPlayerPosition()
{
    // FIXME: This is hardcoded and bad; don't do this
    float distance = 1024.0;
    glm::vec3 camera_direction(1.0f, 1.0f, -0.5f);
    camera_direction = glm::normalize(camera_direction);

    world_Player.GetCameraProperty()->origin.x = 512 + camera_direction.x * distance;
    world_Player.GetCameraProperty()->origin.y = 0   + camera_direction.y * distance;
    world_Player.GetCameraProperty()->origin.z = 512 + camera_direction.z * distance;
}

const Mesh* WorldManager::GetMesh(MeshID mesh_uid)
{
    if(!world_MeshStorage.contains(mesh_uid))
    {
        PRINTERR("WorldManager::GetMesh(MeshID) - invalid Mesh UID; returning nullptr");
        return nullptr;
    }

    return world_MeshStorage.at(mesh_uid).GetMesh();
}

WorldManager::MeshID WorldManager::AddMesh(const Mesh& new_mesh)
{
    int mesh_uid = world_MeshStorage.size();
    // FIXME: This could probably do to be safer...
    world_MeshStorage[mesh_uid] = MeshWrapper(new_mesh, mesh_uid);
    return mesh_uid;
}

void WorldManager::RenderWorld()
{
    GraphicsBackend* current_GraphicsBackend = global_BackendManager->GetGraphicsBackend();

    for(int i = 0 ; i < world_RenderCommandQueue.size() ; i++)
    {
        // This function should contain as few conditionals as possible and just be a straight shot down.
        current_GraphicsBackend->GetShader(Shaders::BLINN_PHONG)->Bind();
    }
}
// The rest are the console command functions (see hl2_src/app/legion/worldmanager.cpp:128-166)
