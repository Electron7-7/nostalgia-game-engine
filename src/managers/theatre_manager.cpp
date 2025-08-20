#include "theatre_manager.hpp"
#include "printing.hpp"
#include "rendering/camera_property.hpp"

#include <glm/glm.hpp>

TheatreManager singleton_TheatreManager;
TheatreManager* global_TheatreManager = &singleton_TheatreManager;

// Console variables for setting the forward and backward speed of the camera (hl2_src/app/legion/worldmanager.cpp:24-25)

TheatreManager::TheatreManager()
{
}

TheatreManager::~TheatreManager()
{
}

TheatreReturnValue_t TheatreManager::TheatreInit(bool is_first_call)
{
    if(!is_first_call)
    { return FINISHED; }

    CreateThings();
    SetInitialLocalPlayerPosition();

    return FINISHED;
}

TheatreReturnValue_t TheatreManager::TheatreShutdown(bool is_first_call)
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

NostalgiaPlayerActor* TheatreManager::GetLocalPlayer()
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

MeshWrapper::MeshID TheatreManager::AddMesh(const Mesh& new_mesh)
{
    for(auto& [mesh_id, mesh] : world_MeshStorage) // FIXME: This might be more dangerous than an iterative for-loop
    {
        if(!mesh.GetMesh()->GetName().compare(new_mesh.GetName()))
        {
            PRINT_ERROR("TheatreManager::AddMesh - A Mesh with the same name \"{}\" already exists! Returning that Mesh's UID", new_mesh.GetName())
            return mesh_id;
        }
    }

    int mesh_uid = world_MeshStorage.size();
    // FIXME: This could probably do to be safer...
    world_MeshStorage[mesh_uid] = MeshWrapper(new_mesh, mesh_uid);
    return mesh_uid;
}

const Mesh* TheatreManager::GetMesh(MeshWrapper::MeshID mesh_uid)
{
    if(!world_MeshStorage.contains(mesh_uid))
    {
        PRINT_ERROR("TheatreManager::GetMesh(MeshID) - invalid Mesh UID; returning nullptr")
        return nullptr;
    }

    return world_MeshStorage.at(mesh_uid).GetMesh();
}

std::map<MeshWrapper::MeshID, const Mesh*> TheatreManager::GetAllCurrentMeshes()
{
    std::map<MeshWrapper::MeshID, const Mesh*> all_meshes;

    for(auto& [mesh_uid, mesh_wrapper] : world_MeshStorage)
        all_meshes[mesh_uid] = mesh_wrapper.GetMesh();

    return all_meshes;
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
