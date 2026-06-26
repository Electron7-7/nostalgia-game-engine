#include "./engine.hpp"
#include "things/thing_factory.hpp"
#include "things/thinkers/3d/collider_3d.hpp"
#include "thirdparty/Jolt/Core/Factory.h"
#include "thirdparty/Jolt/RegisterTypes.h"
#include "thirdparty/Jolt/Physics/PhysicsSystem.h"
#include "thirdparty/Jolt/Core/JobSystemThreadPool.h"
#include "thirdparty/Jolt/Physics/Body/BodyActivationListener.h"
#include "thirdparty/Jolt/Physics/Collision/ObjectLayer.h"
#include "thirdparty/Jolt/Physics/Collision/ContactListener.h"
#include <cassert>
#include <cstdarg>

using namespace JPH;

JPH_SUPPRESS_WARNINGS

////////////////////////////
// BEGIN JOLT BOILERPLATE //
////////////////////////////

// You can add as many layers as you want
namespace Layers
{
    static constexpr ObjectLayer NON_MOVING {0},
        MOVING     {1},
        NUM_LAYERS {2};
}

// Comments from JoltPhysics 'HelloWorld.cpp':
// Each broadphase layer results in a separate bounding volume tree in the broad phase. You at least want to have
// a layer for non-moving and moving objects to avoid having to update a tree full of static objects every frame.
// You can have a 1-on-1 mapping between object layers and broadphase layers (like in this case) but if you have
// many object layers you'll be creating many broad phase trees, which is not efficient. If you want to fine tune
// your broadphase layers define JPH_TRACK_BROADPHASE_STATS and look at the stats reported on the TTY.
namespace BroadPhaseLayers
{
    static constexpr BroadPhaseLayer NON_MOVING {0},
        MOVING {1};
    static constexpr uint NUM_LAYERS{2};
}

class Jolt_ObjectLayerPairFilter : public ObjectLayerPairFilter
{
public:
    virtual bool ShouldCollide(ObjectLayer in_object_1, ObjectLayer in_object_2) const override
    {
        switch(in_object_1)
        {
        case Layers::NON_MOVING:
            return in_object_2 == Layers::MOVING; // Non-moving only collides with moving
        case Layers::MOVING:
            return true; // Moving collides with everything
        default:
            JPH_ASSERT(false);
            return false;
        }
    }
};

class Jolt_ObjectVsBPLayerFilter : public ObjectVsBroadPhaseLayerFilter
{
public:
    virtual bool ShouldCollide(ObjectLayer in_layer_1, BroadPhaseLayer in_layer_2) const override
    {
        switch(in_layer_1)
        {
        case Layers::NON_MOVING:
            return in_layer_2 == BroadPhaseLayers::MOVING;
        case Layers::MOVING:
            return true;
        default:
            JPH_ASSERT(false);
            return false;
        }
    }
};

class Jolt_BPLayerInterface final : public BroadPhaseLayerInterface
{
public:
    Jolt_BPLayerInterface()
    {
        mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
        mObjectToBroadPhase[Layers::MOVING]     = BroadPhaseLayers::MOVING;
    }

    virtual uint GetNumBroadPhaseLayers() const override
    { return BroadPhaseLayers::NUM_LAYERS; }

    virtual BroadPhaseLayer GetBroadPhaseLayer(ObjectLayer in_layer) const override
    {
        JPH_ASSERT(in_layer < Layers::NUM_LAYERS);
        return mObjectToBroadPhase[in_layer];
    }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    virtual const char* GetBroadPhaseLayerName(BroadPhaseLayer in_layer) const override
    {
        switch((BroadPhaseLayer::Type)in_layer)
        {
        case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:
            return "NON_MOVING";
        case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:
            return "MOVING";
        default:
            JPH_ASSERT(false);
            return "INVALID";
        }
    }
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

private:
    BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
};

class Jolt_ContactListener : public ContactListener
{
public:
    virtual ValidateResult OnContactValidate(Farg<Body> inBody1,
        Farg<Body> inBody2,
        RVec3Arg inBaseOffset,
        Farg<CollideShapeResult> inCollisionResult) override
    {
        if(gJoltDebugMessageAllow_ContactValidate)
            { print_jolt("Contact validate callback"); }
        return ValidateResult::AcceptAllContactsForThisBodyPair;
    }

    virtual void OnContactAdded(Farg<Body> inBody1,
        Farg<Body> inBody2,
        Farg<ContactManifold> manifold,
        ContactSettings& ioSettings) override
    {
        if(gJoltDebugMessageAllow_ContactAdded)
            { print_jolt("A contact was added"); }
        auto collider1_id{inBody1.GetUserData()};
        auto collider2_id{inBody2.GetUserData()};
        if(collider1_id and collider2_id and collider1_id != ID::Invalid and collider2_id != ID::Invalid)
        {
            ThingFactory::GetThing<Collider3D>(ID{collider1_id})
                ->OnContactAdded(collider2_id, inBody1, inBody2, manifold, ioSettings);
            ThingFactory::GetThing<Collider3D>(ID{collider2_id})
                ->OnContactAdded(collider1_id, inBody2, inBody1, manifold, ioSettings);
        }
    }

    virtual void OnContactPersisted(Farg<Body> inBody1,
        Farg<Body> inBody2,
        Farg<ContactManifold> manifold,
        ContactSettings& ioSettings) override
    {
        if(gJoltDebugMessageAllow_ContactPersisted)
            { print_jolt("A contact was persisted"); }
        auto collider1_id{inBody1.GetUserData()};
        auto collider2_id{inBody2.GetUserData()};
        if(collider1_id and collider2_id and collider1_id != ID::Invalid and collider2_id != ID::Invalid)
        {
            ThingFactory::GetThing<Collider3D>(ID{collider1_id})
                ->OnContactPersisted(collider2_id, inBody1, inBody2, manifold, ioSettings);
            ThingFactory::GetThing<Collider3D>(ID{collider2_id})
                ->OnContactAdded(collider1_id, inBody2, inBody1, manifold, ioSettings);
        }
    }

    virtual void OnContactRemoved(Farg<SubShapeIDPair> inSubShapeIDPair) override
    {
        if(gJoltDebugMessageAllow_ContactRemoved)
            { print_jolt("A contact was removed"); }
        /*auto collider1_id = PhysicsEngine::Instance()->BodyInterface()
            .GetUserData(inSubShapeIDPair.GetBody1ID());
        if(collider1_id and collider1_id != ID::Invalid)
        {
            ThingFactory::GetThing<Collider3D>(ID{collider1_id})
                ->OnContactRemoved(inSubShapeIDPair);
        }*/
    }
};

class Jolt_BodyActivationListener : public BodyActivationListener
{
public:
    virtual void OnBodyActivated(Farg<BodyID> inBodyID, uint64 inBodyUserData) override
    {
        if(gJoltDebugMessageAllow_BodyActivated)
            { print_jolt("A body was activated [index: {}]", inBodyID.GetIndex()); }
        if(inBodyUserData and inBodyUserData != ID::Invalid)
            { ThingFactory::GetThing<Collider3D>(ID{inBodyUserData})->OnBodyActivated(); }
    }

    virtual void OnBodyDeactivated(Farg<BodyID> inBodyID, uint64 inBodyUserData) override
    {
        if(gJoltDebugMessageAllow_BodyDeactivated)
            { print_jolt("A body went to sleep [index: {}]", inBodyID.GetIndex()); }
        if(inBodyUserData and inBodyUserData != ID::Invalid)
            { ThingFactory::GetThing<Collider3D>(ID{inBodyUserData})->OnBodyDeactivated(); }
    }
};

static void sJoltTraceImpl(const char* inFMT, ...)
{
    va_list list;
    va_start(list, inFMT);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), inFMT, list);
    va_end(list);
    print_jolt("{}", buffer);
}

#ifdef JPH_ENABLE_ASSERTS
static bool sJoltAssertFailed(const char* in_expression,
    const char* in_message,
    const char* in_file,
    uint in_line)
{
    // Quoting Jolt/Core/IssueReporting.h line 9:
    // "This function should return true if a breakpoint needs to be triggered"
    return !print_errorv(VERBOSE0, "{}({}) `{}` {}",
        in_file,
        in_line,
        in_expression,
        (!in_message) ? "" : in_message);
}
#endif // JPH_ENABLE_ASSERTS

//////////////////////////
// END JOLT BOILERPLATE //
//////////////////////////

Shared<PhysicsEngine> PhysicsEngine::m_sInstance{nullptr};

#pragma message("TODO: Implement custom job system and replace `JobSystemThreadPool`")
Jolt_BPLayerInterface       sBroadPhaseLayerInterface{};
Jolt_ObjectVsBPLayerFilter  sObjectVsBroadphaseLayerFilter{};
Jolt_ObjectLayerPairFilter  sObjectVsObjectLayerFilter{};
Jolt_BodyActivationListener sBodyActivationListener{};
Jolt_ContactListener        sContactListener{};

void PhysicsEngine::Instantiate()
{ assert(!m_sInstance); m_sInstance = Shared<PhysicsEngine>{new PhysicsEngine{}}; }

Shared<PhysicsEngine> PhysicsEngine::Instance()
{ assert(m_sInstance); return m_sInstance; }

ObjectLayer PhysicsEngine::GetObjectLayer(MotionType inMotion) noexcept
{
    switch(inMotion)
    {
    case MotionType::Dynamic:
        return Layers::MOVING;
    case MotionType::Kinematic:
        return Layers::MOVING;
    case MotionType::None:
    default:
        print_warning("Invalid motion type detected; defaulting to `Layers::NON_MOVING`");
        [[fallthrough]];
    case MotionType::Static:
        return Layers::NON_MOVING;
    }
}

EMotionType PhysicsEngine::ConvertMotionType(MotionType inMotion) noexcept
{
    switch(inMotion)
    {
    case MotionType::Dynamic:
        return EMotionType::Dynamic;
    case MotionType::Kinematic:
        return EMotionType::Kinematic;
    case MotionType::None:
    default:
        print_warning("Invalid motion type detected; defaulting to `JPH::EMotionType::Static`");
        [[fallthrough]];
    case MotionType::Static:
        return EMotionType::Static;
    }
}

EActivation PhysicsEngine::GetActivation(bool setActive) noexcept
{
    return (setActive)
        ? EActivation::Activate
        : EActivation::DontActivate;
}

PhysicsEngine::PhysicsEngine() noexcept
{
    RegisterDefaultAllocator();

    Trace = sJoltTraceImpl;
    JPH_IF_ENABLE_ASSERTS(AssertFailed = sJoltAssertFailed;)

    m_pTempAllocator = new TempAllocatorImpl{10 * 1024 * 1024};
    m_pJobSystem     = new JobSystemThreadPool{cMaxPhysicsJobs, cMaxPhysicsBarriers};
}

PhysicsEngine::~PhysicsEngine() noexcept
{
    delete m_pJobSystem;
    m_pJobSystem = nullptr;

    delete m_pTempAllocator;
    m_pTempAllocator = nullptr;
}

Error PhysicsEngine::Start()
{ return Start(65536, 0, 65536, 10240); }

Error PhysicsEngine::Start(uint inMaxBodies,
    uint inNumBodyMutexes,
    uint inMaxBodyPairs,
    uint inMaxContactConstraints)
{
    Factory::sInstance = new Factory();
    RegisterTypes();

    m_pSystem = new PhysicsSystem;
    m_pSystem->Init(inMaxBodies,
        inNumBodyMutexes, // setting to 0 uses default settings
        inMaxBodyPairs,
        inMaxContactConstraints,
        sBroadPhaseLayerInterface,
        sObjectVsBroadphaseLayerFilter,
        sObjectVsObjectLayerFilter);
    m_pSystem->SetBodyActivationListener(&sBodyActivationListener);
    m_pSystem->SetContactListener(&sContactListener);
    return OK;
}

void PhysicsEngine::Stop()
{
    UnregisterTypes();

    delete Factory::sInstance;
    Factory::sInstance = nullptr;

    delete m_pSystem;
    m_pSystem = nullptr;
}

void PhysicsEngine::Tick(float inInterval, int inSteps)
{ m_pSystem->Update(inInterval, inSteps, m_pTempAllocator, m_pJobSystem); }

PhysicsSystem* PhysicsEngine::System()
{ assert(m_pSystem); return m_pSystem; }

BodyInterface& PhysicsEngine::BodyInterface()
{ assert(m_pSystem); return m_pSystem->GetBodyInterface(); }

BodyLockInterfaceArg PhysicsEngine::BodyLockInterface() const
{ assert(m_pSystem); return m_pSystem->GetBodyLockInterface(); }
