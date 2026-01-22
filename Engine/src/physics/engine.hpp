#ifndef PHYSICS_ENGINE_H
#define PHYSICS_ENGINE_H

#include <Jolt/Jolt.h>
#include <Jolt/Math/Vec3.h>
#include <Jolt/Math/Quat.h>
#include <Jolt/Math/DVec3.h>
#include <Jolt/Math/Float2.h>
#include <Jolt/Core/Atomics.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Core/JobSystem.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyLockInterface.h>
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Physics/EActivation.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>

// Forward Declaration
enum class MotionType : ushort;

using BodyArg              = const JPH::Body&;
using BodyIDArg            = const JPH::BodyID&;
using BodyLockInterfaceArg = const JPH::BodyLockInterface&;

inline bool
    gJoltDebugMessageAllow_ContactValidate  {false},
    gJoltDebugMessageAllow_ContactAdded     {false},
    gJoltDebugMessageAllow_ContactPersisted {false},
    gJoltDebugMessageAllow_ContactRemoved   {false},
    gJoltDebugMessageAllow_BodyActivated    {false},
    gJoltDebugMessageAllow_BodyDeactivated  {false};

class PhysicsEngine
{
public:
    ~PhysicsEngine() noexcept;

    Error Start();
    Error Start(uint inMaxBods, uint inNumBodMutx, uint inMaxBodPairs, uint inMaxContactConstr);
    void  Stop();
    void  Tick(float inTickInterval, int inCollisionSteps = 1);

    JPH::PhysicsSystem*  System();
    JPH::BodyInterface&  BodyInterface();
    BodyLockInterfaceArg BodyLockInterface() const;
    JPH::BodyID CreateAndAddBody(ID inColliderID,
        Shared<JPH::BodyCreationSettings> inCreationSettings,
        JPH::EActivation inActivation);

    static JPH::ObjectLayer GetObjectLayer(MotionType) noexcept;
    static JPH::EMotionType ConvertMotionType(MotionType) noexcept;
    static void Instantiate();
    static Shared<PhysicsEngine> Instance();
    static Shared<PhysicsEngine> Inst() noexcept;

private:
    static Shared<PhysicsEngine> m_sInstance;

    // github.com/jrouwe/JoltPhysics/blob/master/UnitTests/PhysicsTestContext.h:112-117
    JPH::TempAllocator* m_pTempAllocator{nullptr};
    JPH::JobSystem*     m_pJobSystem{nullptr};
    JPH::PhysicsSystem* m_pSystem{nullptr};

    PhysicsEngine() noexcept;
};

#endif // PHYSICS_ENGINE_H
