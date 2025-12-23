#ifndef PHYSICS_ENUMS
#define PHYSICS_ENUMS
    enum class PhysicsBodyShape : unsigned short
    { Box, Sphere, Capsule, Cylinder };

    enum class PhysicsBodyMotion : unsigned short
    { Static, Dynamic, Kinematic };
#endif // PHYSICS_ENUMS
#ifdef FWD_DCL
    namespace JPH
    {
        class BodyInterface;
        class BodyID;
        class TempAllocator;
        class JobSystem;
        class PhysicsSystem;
        class Float2;
    }
    class PhysicsManager;
    extern PhysicsManager* g_pPhysicsManager;
    extern bool gEnableMsg_ContactValidate,
        gEnableMsg_ContactAdded,
        gEnableMsg_ContactPersisted,
        gEnableMsg_ContactRemoved,
        gEnableMsg_BodyActivated,
        gEnableMsg_BodyDeactivated;
#elif !defined PHYSICS_MANAGER_H
#define PHYSICS_MANAGER_H

#define FWD_DCL
#   include "physics_manager.hpp"
#   include "things/devices/collider.hpp"
#undef  FWD_DCL

#include "manager.hpp"
#include "core/id.hpp"
#include "core/smart_pointers.hpp"
/////////////////////////
// Begin Jolt Boilerplate
// See: github.com/jrouwe/JoltPhysics/blob/master/UnitTests/UnitTestFramework.h
// Note: I use angle includes here to suppress clang's "unused header" warnings
#include <Jolt/Jolt.h>
#include <Jolt/Core/Atomics.h>
#include <Jolt/Math/DVec3.h>
#include <Jolt/Math/Float2.h>
#include <Jolt/Physics/Body/BodyID.h>
// Disable common warnings
JPH_SUPPRESS_WARNINGS
JPH_CLANG_SUPPRESS_WARNING("-Wheader-hygiene")
#ifdef JPH_DOUBLE_PRECISION
JPH_CLANG_SUPPRESS_WARNING("-Wdouble-promotion")
#endif // JPH_DOUBLE_PRECISION
// End Jolt Boilerplate
///////////////////////

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
    bool ValidateColliderUID(ID UID, Shared<Collider> Ouptut, bool ValidateBodyID);

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
