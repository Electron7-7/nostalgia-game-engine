#ifndef PHYSICS_MANAGER_H
#define PHYSICS_MANAGER_H

#include <Nostalgia/managers/manager.hpp>
#include <glm/fwd.hpp>

class PhysicsManager : public Manager
{
public:
    consteval const char* DebugName() { return "PhysicsManager"; }
    bool Init();
    ManagerEnums::TheatreReturnValue_t TheatreInit(bool IsFirstCall);
    void Shutdown();
    ManagerEnums::TheatreReturnValue_t TheatreShutdown(bool IsFirstCall);
    void Tick();
};

extern PhysicsManager* g_pPhysicsManager;

#endif // PHYSICS_MANAGER_H
