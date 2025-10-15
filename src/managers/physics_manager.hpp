#ifndef PHYSICS_MANAGER_H
#define PHYSICS_MANAGER_H

#include "manager.hpp"
#include "ids.hpp"
#include "Jolt/Jolt.h" // IWYU pragma: keep
#include "Jolt/Physics/PhysicsSystem.h"

extern bool gEnableMsg_ContactValidate;
extern bool gEnableMsg_ContactAdded;
extern bool gEnableMsg_ContactPersisted;
extern bool gEnableMsg_ContactRemoved;
extern bool gEnableMsg_BodyActivated;
extern bool gEnableMsg_BodyDeactivated;

class PhysicsManager : public Manager
{
public:
    bool Init();
    ManagerEnums::TheatreReturnValue_t TheatreInit(bool IsFirstCall);
    void Shutdown();
    ManagerEnums::TheatreReturnValue_t TheatreShutdown(bool IsFirstCall);
    void Tick();

    JPH::PhysicsSystem* Jolt() const;
    JPH::BodyInterface& GetBodyInterface() const;
    JPH::BodyID& GetBodyID(const ID& UID) const;

    bool CreateBody(const ID& UID) const;
    void DestroyBody(const ID& UID) const;

private:
    // github.com/jrouwe/JoltPhysics/blob/master/UnitTests/PhysicsTestContext.h:112-117
    JPH::TempAllocator* mTempAllocator{nullptr};
    JPH::JobSystem* mJobSystem{nullptr};
    JPH::PhysicsSystem* mSystem{nullptr};
};

extern PhysicsManager* g_pPhysicsManager;

#endif // PHYSICS_MANAGER_H
