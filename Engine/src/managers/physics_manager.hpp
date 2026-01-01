#ifndef PHYSICS_MANAGER_H
#define PHYSICS_MANAGER_H

#include "fwd/physics.hpp"
#include "fwd/theatre.hpp"
#include "managers/manager.hpp"
#include "core/id.hpp"

class PhysicsManager : public Manager
{
public:
    consteval const char* DebugName() { return "PhysicsManager"; }
    bool Init();
    ManagerEnums::TheatreReturnValue_t TheatreInit(bool IsFirstCall);
    void Shutdown();
    ManagerEnums::TheatreReturnValue_t TheatreShutdown(bool IsFirstCall);
    void Tick();

    JPH::BodyInterface& GetBodyInterface();
    JPH::BodyID& GetBodyID(ID UID);

    bool CreateBody(ID UID, Farg<Transform3D> inTransform, PhysicsBodyShape, PhysicsBodyMotion);
    bool DestroyBody(ID UID);

private:
    bool ValidateColliderUID(ID UID, bool ValidateBodyID);

    // github.com/jrouwe/JoltPhysics/blob/master/UnitTests/PhysicsTestContext.h:112-117
    JPH::TempAllocator* mTempAllocator{nullptr};
    JPH::JobSystem* mJobSystem{nullptr};
    JPH::PhysicsSystem* mSystem{nullptr};
};

extern PhysicsManager* g_pPhysicsManager;

extern bool gEnableMsg_ContactValidate,
    gEnableMsg_ContactAdded,
    gEnableMsg_ContactPersisted,
    gEnableMsg_ContactRemoved,
    gEnableMsg_BodyActivated,
    gEnableMsg_BodyDeactivated;

#endif // PHYSICS_MANAGER_H
