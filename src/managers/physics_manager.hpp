#ifndef PHYSICS_MANAGER_H
#define PHYSICS_MANAGER_H

#include "things/fwd.hpp"
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

    JPH::BodyInterface& GetBodyInterface();
    JPH::BodyID& GetBodyID(ID UID);

    /// If `Collider` is `nullptr`, more work is done to verify and get the Collider at `UID`
    bool CreateBody(ID UID,  std::shared_ptr<Collider> Collider = nullptr);
    /// If `Collider` is `nullptr`, more work is done to verify and get the Collider at `UID`
    bool DestroyBody(ID UID, std::shared_ptr<Collider> Collider = nullptr);

private:
    bool ValidateColliderUID(ID UID, std::shared_ptr<Collider> Ouptut, bool ValidateBodyID);

    // github.com/jrouwe/JoltPhysics/blob/master/UnitTests/PhysicsTestContext.h:112-117
    JPH::TempAllocator* mTempAllocator{nullptr};
    JPH::JobSystem* mJobSystem{nullptr};
    JPH::PhysicsSystem* mSystem{nullptr};
};

extern PhysicsManager* g_pPhysicsManager;

#endif // PHYSICS_MANAGER_H
