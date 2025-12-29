#ifndef PHYSICS_MANAGER_H
#define PHYSICS_MANAGER_H

#include "fwd/things.hpp"
#include "fwd/physics.hpp"
#include "managers/manager.hpp"
#include "core/id.hpp"
#include "core/smart_pointers.hpp"

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

    // If `Collider` is `nullptr`, more work is done to verify and get the Collider at `UID`
    bool CreateBody(ID UID,  Shared<Collider> Collider = nullptr);
    // If `Collider` is `nullptr`, more work is done to verify and get the Collider at `UID`
    bool DestroyBody(ID UID, Shared<Collider> Collider = nullptr);

private:
    bool ValidateColliderUID(ID UID, Shared<Collider>& Ouptut, bool ValidateBodyID);

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
