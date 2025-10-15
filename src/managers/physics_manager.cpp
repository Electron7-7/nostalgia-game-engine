#include "physics_manager.hpp"
#include "managers/theatre_manager.hpp"
#include "things/thing_factory.hpp"
#include "settings/settings.hpp"
#include "things/devices/collider.hpp"
#include "printing.hpp"
#include "math/conversion.hpp"
#include "physics/enums.hpp"

#include "Jolt/RegisterTypes.h"
#include "Jolt/Physics/PhysicsSettings.h"
#include "Jolt/Core/JobSystemThreadPool.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"
#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"
#include "Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include "Jolt/Physics/Collision/Shape/CylinderShape.h"
#include "Jolt/Physics/Collision/ObjectLayer.h"
#include "Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h"
#include "Jolt/Physics/Collision/ContactListener.h"
#include "Jolt/Physics/Body/BodyActivationListener.h"

#include <cstdarg>

using namespace JPH;

JPH_SUPPRESS_WARNINGS

bool gEnableMsg_ContactValidate  = false;
bool gEnableMsg_ContactAdded     = false;
bool gEnableMsg_ContactPersisted = false;
bool gEnableMsg_ContactRemoved   = false;
bool gEnableMsg_BodyActivated    = false;
bool gEnableMsg_BodyDeactivated  = false;

//
// BEGIN JOLT BOILERPLATE
//

// You can add as many layers as you want
namespace Layers
{
    static constexpr ObjectLayer NON_MOVING = 0;
    static constexpr ObjectLayer MOVING     = 1;
    static constexpr ObjectLayer NUM_LAYERS = 2;
}

// Comments from JoltPhysics `HelloWorld.cpp`:
// Each broadphase layer results in a separate bounding volume tree in the broad phase. You at least want to have
// a layer for non-moving and moving objects to avoid having to update a tree full of static objects every frame.
// You can have a 1-on-1 mapping between object layers and broadphase layers (like in this case) but if you have
// many object layers you'll be creating many broad phase trees, which is not efficient. If you want to fine tune
// your broadphase layers define JPH_TRACK_BROADPHASE_STATS and look at the stats reported on the TTY.
namespace BroadPhaseLayers
{
    static constexpr BroadPhaseLayer NON_MOVING(0);
    static constexpr BroadPhaseLayer MOVING(1);
    static constexpr uint            NUM_LAYERS(2);
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
    virtual ValidateResult OnContactValidate(const Body& in_body_1, const Body& in_body_2, RVec3Arg in_base_offset, const CollideShapeResult& in_collision_result) override
    {
        if(gEnableMsg_ContactValidate)
            { print_jolt_debug("Contact validate callback"); }
        return ValidateResult::AcceptAllContactsForThisBodyPair;
    }

    virtual void OnContactAdded(const Body& body1, const Body& body2, const ContactManifold& manifold, ContactSettings& io_settings) override
    {
        if(gEnableMsg_ContactAdded)
            { print_jolt_debug("A contact was added"); }
    }

    virtual void OnContactPersisted(const Body& body1, const Body& body2, const ContactManifold& manifold, ContactSettings& io_settings) override
    {
        if(gEnableMsg_ContactPersisted)
            { print_jolt_debug("A contact was persisted"); }
    }

    virtual void OnContactRemoved(const SubShapeIDPair& sub_shape_pair) override
    {
        if(gEnableMsg_ContactRemoved)
            { print_jolt_debug("A contact was removed"); }
    }
};

class Jolt_BodyActivationListener : public BodyActivationListener
{
public:
    virtual void OnBodyActivated(const BodyID& body_id, uint64 body_user_data) override
    {
        if(gEnableMsg_BodyActivated)
            { print_jolt_debug("A body was activated [index: {}]", body_id.GetIndex()); }
    }

    virtual void OnBodyDeactivated(const BodyID& body_id, uint64 body_user_data) override
    {
        if(gEnableMsg_BodyDeactivated)
            { print_jolt_debug("A body went to sleep [index: {}]", body_id.GetIndex()); }
    }
};

static void sJoltTraceImpl(const char* inFMT, ...)
{
    va_list list;
    va_start(list, inFMT);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), inFMT, list);
    va_end(list);
    print_jolt_debug("{}", buffer);
}

#ifdef JPH_ENABLE_ASSERTS
static bool sJoltAssertFailed(const char* in_expression, const char* in_message, const char* in_file, uint in_line)
{
    print_error("{}:{}: ({}){}",
        in_file,
        in_line,
        in_expression,
        (!in_message) ? "" : in_message);
    return true; // Quoting Jolt/Core/IssueReporting.h line 9: "This function should return true if a breakpoint needs to be triggered"
}
#endif // JPH_ENABLE_ASSERTS

//
// END JOLT BOILERPLATE
//

#pragma message("FIXME: Implement custom job system and replace `JobSystemThreadPool`")

Jolt_BPLayerInterface       sBroadPhaseLayerInterface;
Jolt_ObjectVsBPLayerFilter  sObjectVsBroadphaseLayerFilter;
Jolt_ObjectLayerPairFilter  sObjectVsObjectLayerFilter;
Jolt_BodyActivationListener sBodyActivationListener;
Jolt_ContactListener        sContactListener;

static std::map<ID, BodyID> sBodyIDMap{};

static PhysicsManager sPhysicsManager;
PhysicsManager* g_pPhysicsManager = &sPhysicsManager;

#pragma message("FIXME: Read JoltPhysics `HelloWorld.cpp` and make the necessary changes as you implement more of the engine")
bool PhysicsManager::Init()
{
    RegisterDefaultAllocator();

    Trace = sJoltTraceImpl;
    JPH_IF_ENABLE_ASSERTS(AssertFailed = sJoltAssertFailed;)

    mTempAllocator = new TempAllocatorImpl(10 * 1024 * 1024);
    mJobSystem     = new JobSystemThreadPool(cMaxPhysicsJobs, cMaxPhysicsBarriers);

    return true;
}

void PhysicsManager::Shutdown()
{
    delete mJobSystem;
    mJobSystem = nullptr;

    delete mTempAllocator;
    mTempAllocator = nullptr;
}

ManagerEnums::TheatreReturnValue_t PhysicsManager::TheatreInit(bool is_first_call)
{
    if(!is_first_call)
        { return ManagerEnums::FINISHED; }

    Factory::sInstance = new Factory();
    RegisterTypes();

    const uint cMaxBodies = 65536;
    const uint cNumBodyMutexes = 0; // 0 == use default settings
    const uint cMaxBodyPairs = 65536;
    const uint cMaxContactConstraints = 10240;

    mSystem = new PhysicsSystem;
    mSystem->Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, sBroadPhaseLayerInterface, sObjectVsBroadphaseLayerFilter, sObjectVsObjectLayerFilter);

    mSystem->SetBodyActivationListener(&sBodyActivationListener);
    mSystem->SetContactListener(&sContactListener);
    // mSystem->OptimizeBroadPhase();

    return ManagerEnums::FINISHED;
}

ManagerEnums::TheatreReturnValue_t PhysicsManager::TheatreShutdown(bool is_first_call)
{
    if(!is_first_call)
        { return ManagerEnums::FINISHED; }

    sBodyIDMap.clear();
    UnregisterTypes();

    delete Factory::sInstance;
    Factory::sInstance = nullptr;

    delete mSystem;
    mSystem = nullptr;

    return ManagerEnums::FINISHED;
}

void PhysicsManager::Tick()
{
    if(_Manager::GetTheatreState() != ManagerEnums::IN_LEVEL)
        { return; }
    mSystem->Update(Settings::Engine::TickInterval(), 1, mTempAllocator, mJobSystem);
}

PhysicsSystem* PhysicsManager::Jolt() const
{ assert(mSystem); return mSystem; }

BodyInterface& PhysicsManager::GetBodyInterface() const
{ assert(mSystem); return mSystem->GetBodyInterface(); }

BodyID& PhysicsManager::GetBodyID(const ID& uid) const
{ return sBodyIDMap[uid]; }

void PhysicsManager::DestroyBody(const ID& uid) const
{
    if(!mSystem || sBodyIDMap[uid].IsInvalid())
        { return; }
    mSystem->GetBodyInterface().RemoveBody(sBodyIDMap[uid]);
    mSystem->GetBodyInterface().DestroyBody(sBodyIDMap[uid]);
}

bool PhysicsManager::CreateBody(const ID& uid) const
{
    if(!g_pTheatreManager->ThingExists(uid))
        { return print_error("PhysicsManager::CreatePhysicsBody - invalid UID: {}", uid); }
    else if(!g_pThingFactory->IsDerivedFrom<Collider>(g_pTheatreManager->GetType(uid)))
        { return print_error("PhysicsManager::CreatePhysicsBody - invalid type: {}", g_pThingFactory->GetTypeName(g_pTheatreManager->GetType(uid))); }

    auto  collider = g_pTheatreManager->GetThing<Collider>(uid);
    auto& interface = Jolt()->GetBodyInterface();

    BodyCreationSettings settings;
    EMotionType motion_type;
    ObjectLayer layer;

    switch(collider->Motion())
    {
    case PhysicsBodyMotion::Dynamic:
        motion_type = EMotionType::Dynamic;
        layer = Layers::MOVING;
        break;
    case PhysicsBodyMotion::Kinematic:
        motion_type = EMotionType::Kinematic;
        layer = Layers::MOVING;
        break;
    case PhysicsBodyMotion::Static:
    default:
        motion_type = EMotionType::Static;
        layer = Layers::NON_MOVING;
        break;
    }

    RVec3 scale{GlmToJolt<Vec3>(collider->Scale())};
    RVec3 position{GlmToJolt<Vec3>(collider->Origin())};
    Quat rotation{JPH::Quat::sEulerAngles(GlmToJolt<Vec3>(collider->Euler()))};

    switch(collider->Shape())
    {
    case PhysicsBodyShape::Box:
        settings = BodyCreationSettings(
            new BoxShapeSettings(scale),
            position,
            rotation,
            motion_type,
            layer);
        break;
    case PhysicsBodyShape::Sphere:
        settings = BodyCreationSettings(
            new SphereShapeSettings(scale.GetX() / 2.0f),
            position,
            rotation,
            motion_type,
            layer);
        break;
    case PhysicsBodyShape::Cylinder:
        settings = BodyCreationSettings(
            new CylinderShapeSettings(scale.GetZ() / 2.0f, scale.GetX() / 2.0f),
            position,
            rotation,
            motion_type,
            layer);
        break;
    case PhysicsBodyShape::Capsule:
        settings = BodyCreationSettings(
            new CapsuleShapeSettings(scale.GetZ() / 2.0f, scale.GetX() / 2.0f),
            position,
            rotation,
            motion_type,
            layer);
        break;
    default:
        print_error("PhysicsManager::CreatePhysicsBody - invalid physics body type '{}' from {}#{}", collider->Shape().name(), g_pThingFactory->GetTypeName(collider->type()), uid);
        return false;
    }
#pragma message("TODO: Remove bodies when exiting Theatre and I think the bug with static body is due to setup?")
#pragma message("TODO: Add bodies in a batch and activate them in a batch")
    sBodyIDMap[uid] = interface.CreateAndAddBody(settings, EActivation::Activate);
    print_debug("Jolt Body Created\n\t- Type: {}\n\t- Shape: {}\n\tMotion: {}",
        g_pThingFactory->GetTypeName(collider->type()),
        collider->Shape().name(),
        collider->Motion().name());
    return !sBodyIDMap.at(uid).IsInvalid();
}
