#ifndef THEATRE_MANAGER_H
#define THEATRE_MANAGER_H

#include "manager.hpp"
#include <Macros/forward_declaration.hpp>

// FORWARD_DECLARE(class HeightField)
FORWARD_DECLARE(class CameraProperty)

// Actor type
class NostalgiaPlayerActor
{
public:
    CameraProperty* player_CameraProperty;
};

class WorldManager : public Manager<>
{
public:
    WorldManager();
    virtual ~WorldManager();

    virtual TheatreReturnValue_t TheatreInit(bool IsFirstCall);
    virtual TheatreReturnValue_t TheatreShutdown(bool IsFirstCall);

    void DrawWorld();

    NostalgiaPlayerActor* GetLocalPlayer();

private:
    // Various console commands created via Macro; see hl2_src/external/vpc/public/tier1/convar.h (~line 958) for more details

    // Creates, destroys things
    void CreateThings();
    void DestroyThings();

    // Sets the initial camera position
    void SetInitialLocalPlayerPosition();

    // HeightField* world_HeightField;
    NostalgiaPlayerActor world_Player;
};

// Singleton accessor
extern WorldManager* global_WorldManager;

#endif // THEATRE_MANAGER_H