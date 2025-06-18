#include "world_manager.hpp"
#include "backend_manager.hpp"
#include "backends/backend.hpp"
#include "rendering/camera_property.hpp"
#include "common/debugging.hpp"
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

    global_BackendManager->GetGraphicsBackend()->InitNewTheatre();

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

MeshWrapper::MeshID WorldManager::AddMesh(const Mesh& new_mesh)
{
    for(auto& [mesh_id, mesh] : world_MeshStorage) // FIXME: This might be more dangerous than an iterative for-loop
    {
        if(!mesh.GetMesh()->GetName().compare(new_mesh.GetName()))
        {
            PRINTERR("WorldManager::AddMesh - A Mesh with the same name \"" + new_mesh.GetName() + "\" already exists! Returning that Mesh's UID");
            return mesh_id;
        }
    }

    int mesh_uid = world_MeshStorage.size();
    // FIXME: This could probably do to be safer...
    world_MeshStorage[mesh_uid] = MeshWrapper(new_mesh, mesh_uid);
    return mesh_uid;
}

const Mesh* WorldManager::GetMesh(MeshWrapper::MeshID mesh_uid)
{
    if(!world_MeshStorage.contains(mesh_uid))
    {
        PRINTERR("WorldManager::GetMesh(MeshID) - invalid Mesh UID; returning nullptr");
        return nullptr;
    }

    return world_MeshStorage.at(mesh_uid).GetMesh();
}

std::map<MeshWrapper::MeshID, const Mesh*> WorldManager::GetAllCurrentMeshes()
{
    std::map<MeshWrapper::MeshID, const Mesh*> all_meshes;

    for(auto& [mesh_uid, mesh_wrapper] : world_MeshStorage)
        all_meshes[mesh_uid] = mesh_wrapper.GetMesh();

    return all_meshes;
}

void WorldManager::RenderWorld()
{
    // for(int i = 0 ; i < world_RenderCommandQueue.size() ; i++)
    // {
        // global_BackendManager->GetGraphicsBackend()->RenderSingleCommand(world_RenderCommandQueue.at(i));
        // world_RenderCommandQueue.erase(world_RenderCommandQueue.begin() + i);
    // }
}
// The rest are the console command functions (see hl2_src/app/legion/worldmanager.cpp:128-166)
