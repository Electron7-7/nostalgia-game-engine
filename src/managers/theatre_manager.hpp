#ifndef THEATRE_MANAGER_H
#define THEATRE_MANAGER_H

#include "manager.hpp"
#include "rendering/mesh.hpp"
#include "rendering/render_command.hpp"
#include "things/actors/nostalgia_player.hpp"

#include <list>

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

class TheatreManager : public Manager
{
public:
    TheatreManager();
    ~TheatreManager();

    NostalgiaPlayer* GetLocalPlayer();

    void RenderWorld();
    ManagerEnums::TheatreReturnValue_t TheatreInit(bool IsFirstCall);
    ManagerEnums::TheatreReturnValue_t TheatreShutdown(bool IsFirstCall);

private:
    // Various console commands created via Macro; see hl2_src/external/vpc/public/tier1/convar.h (~line 958) for more details

    // Creates, destroys things
    void CreateThings();
    void DestroyThings();

    // Sets the initial camera position
    void SetInitialLocalPlayerPosition();

    NostalgiaPlayer world_Player;

    std::vector<RenderCommand> m_RenderCommandQueue = {};
    std::list<std::unique_ptr<Thing>> m_ThingStorage = {};
    std::map<unsigned int, MeshWrapper> m_MeshStorage = {};
};

// Singleton accessor
extern TheatreManager* global_TheatreManager;

#endif // THEATRE_MANAGER_H
