#ifndef THEATRE_MANAGER_H
#define THEATRE_MANAGER_H

#include "manager.hpp"
#include "rendering/mesh.hpp"
#include "rendering/render_command.hpp"
#include "things/actors/nostalgia_player.hpp"

#include <memory>

struct MeshWrapper
{
public:
    typedef int MeshID;

    MeshWrapper(const Mesh& Mesh, const int UID): _mesh(Mesh), _uid(UID) {}
    MeshWrapper(): MeshWrapper(Mesh(), 0) {}

    int GetUID() const { return _uid; }
    Mesh* GetMesh() { return &_mesh; }

private:
    Mesh _mesh;
    MeshID _uid = 0;
};

class TheatreManager : public Manager<>
{
public:
    TheatreManager();
    ~TheatreManager();
    TheatreReturnValue_t TheatreInit(bool IsFirstCall);
    TheatreReturnValue_t TheatreShutdown(bool IsFirstCall);

    NostalgiaPlayerActor* GetLocalPlayer();

    MeshWrapper::MeshID AddMesh(const Mesh& NewMesh); // Returns the integer UID assigned to the new Mesh
    const Mesh* GetMesh(int MeshID);
    std::map<MeshWrapper::MeshID, const Mesh*> GetAllCurrentMeshes();

    void RenderWorld();

private:
    // Various console commands created via Macro; see hl2_src/external/vpc/public/tier1/convar.h (~line 958) for more details

    // Creates, destroys things
    void CreateThings();
    void DestroyThings();

    // Sets the initial camera position
    void SetInitialLocalPlayerPosition();

    NostalgiaPlayerActor world_Player;

    std::vector<RenderCommand> world_RenderCommandQueue = {};
    std::vector<std::unique_ptr<Thing>> world_ThingStorage = {};
    std::map<MeshWrapper::MeshID, MeshWrapper> world_MeshStorage = {};
};

// Singleton accessor
extern TheatreManager* global_TheatreManager;

#endif // THEATRE_MANAGER_H
