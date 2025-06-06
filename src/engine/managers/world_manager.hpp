#ifndef THEATRE_MANAGER_H
#define THEATRE_MANAGER_H

#include "manager.hpp"
#include "engine/rendering/mesh.hpp"
#include "engine/rendering/render_command.hpp"
#include "engine/things/actors/nostalgia_player.hpp"
#include <memory>

struct MeshWrapper
{
public:
    MeshWrapper(const Mesh& Mesh, const int UID): _mesh(Mesh), _uid(UID) {}
    MeshWrapper(): MeshWrapper(Mesh(), 0) {}

    int GetUID() const { return _uid; }
    Mesh* GetMesh() { return &_mesh; }

private:
    Mesh _mesh;
    int _uid = 0;
};

class WorldManager : public Manager<>
{
public:
    typedef int MeshID;

    WorldManager();
    virtual ~WorldManager();

    virtual TheatreReturnValue_t TheatreInit(bool IsFirstCall);
    virtual TheatreReturnValue_t TheatreShutdown(bool IsFirstCall);

    NostalgiaPlayerActor* GetLocalPlayer();

    const Mesh* GetMesh(int MeshID);
    MeshID AddMesh(const Mesh& NewMesh); // Returns the integer UID assigned to the new Mesh

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
    std::map<MeshID, MeshWrapper> world_MeshStorage = {};
};

// Singleton accessor
extern WorldManager* global_WorldManager;

#endif // THEATRE_MANAGER_H
