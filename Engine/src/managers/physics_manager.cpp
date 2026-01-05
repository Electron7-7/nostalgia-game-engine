#include "physics_manager.hpp"
#include "core/printing.hpp"
#include "core/enum_prettifier.hpp"
#include "things/devices/collider.hpp"
#include "settings/engine.hpp"
#include "math/conversion.hpp"
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Body/BodyLockInterface.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <cstdarg>
#include <map>

using namespace JPH;

JPH_SUPPRESS_WARNINGS

bool gEnableMsg_ContactValidate  = false;
bool gEnableMsg_ContactAdded     = false;
bool gEnableMsg_ContactPersisted = false;
bool gEnableMsg_ContactRemoved   = false;
bool gEnableMsg_BodyActivated    = false;
bool gEnableMsg_BodyDeactivated  = false;

////////////////////////////
// BEGIN JOLT BOILERPLATE //
////////////////////////////

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
            { print_jolt("Contact validate callback"); }
        return ValidateResult::AcceptAllContactsForThisBodyPair;
    }

    virtual void OnContactAdded(const Body& body1, const Body& body2, const ContactManifold& manifold, ContactSettings& io_settings) override
    {
        if(gEnableMsg_ContactAdded)
            { print_jolt("A contact was added"); }
    }

    virtual void OnContactPersisted(const Body& body1, const Body& body2, const ContactManifold& manifold, ContactSettings& io_settings) override
    {
        if(gEnableMsg_ContactPersisted)
            { print_jolt("A contact was persisted"); }
    }

    virtual void OnContactRemoved(const SubShapeIDPair& sub_shape_pair) override
    {
        if(gEnableMsg_ContactRemoved)
            { print_jolt("A contact was removed"); }
    }
};

class Jolt_BodyActivationListener : public BodyActivationListener
{
public:
    virtual void OnBodyActivated(const BodyID& body_id, uint64 body_user_data) override
    {
        if(gEnableMsg_BodyActivated)
            { print_jolt("A body was activated [index: {}]", body_id.GetIndex()); }
    }

    virtual void OnBodyDeactivated(const BodyID& body_id, uint64 body_user_data) override
    {
        if(gEnableMsg_BodyDeactivated)
            { print_jolt("A body went to sleep [index: {}]", body_id.GetIndex()); }
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

#pragma message("TODO: Implement custom job system and replace `JobSystemThreadPool`")
Jolt_BPLayerInterface       sBroadPhaseLayerInterface;
Jolt_ObjectVsBPLayerFilter  sObjectVsBroadphaseLayerFilter;
Jolt_ObjectLayerPairFilter  sObjectVsObjectLayerFilter;
Jolt_BodyActivationListener sBodyActivationListener;
Jolt_ContactListener        sContactListener;

static std::map<ID, BodyID> sBodyIDMap{};

static PhysicsManager sPhysicsManager;
PhysicsManager* g_pPhysicsManager{&sPhysicsManager};

// TODO: Read JoltPhysics `HelloWorld.cpp` and make the necessary changes as you implement more of the engine
bool PhysicsManager::Init()
{
    PRINT_PRETTY_FUNCTION;
    RegisterDefaultAllocator();

    Trace = sJoltTraceImpl;
    JPH_IF_ENABLE_ASSERTS(AssertFailed = sJoltAssertFailed;)

    mTempAllocator = new TempAllocatorImpl{10 * 1024 * 1024};
    mJobSystem     = new JobSystemThreadPool{cMaxPhysicsJobs, cMaxPhysicsBarriers};

#ifdef DEBUGGING
    EnumPrettifier::Set("Box",       PhysicsBodyShape::Box,        "PhysicsBodyShape");
    EnumPrettifier::Set("Sphere",    PhysicsBodyShape::Sphere,     "PhysicsBodyShape");
    EnumPrettifier::Set("Capsule",   PhysicsBodyShape::Capsule,    "PhysicsBodyShape");
    EnumPrettifier::Set("Cylinder",  PhysicsBodyShape::Cylinder,   "PhysicsBodyShape");
    EnumPrettifier::Set("Dynamic",   PhysicsBodyMotion::Dynamic,   "PhysicsBodyMotion");
    EnumPrettifier::Set("Kinematic", PhysicsBodyMotion::Kinematic, "PhysicsBodyMotion");
    EnumPrettifier::Set("Static",    PhysicsBodyMotion::Static,    "PhysicsBodyMotion");
#endif // DEBUGGING

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
    if(IManager::GetTheatreState() != ManagerEnums::IN_LEVEL)
        { return; }
    mSystem->Update(Settings::Engine::TickInterval(), 1, mTempAllocator, mJobSystem);
}

BodyInterface& PhysicsManager::GetBodyInterface()
{ assert(mSystem); return mSystem->GetBodyInterface(); }

Farg<BodyLockInterface> PhysicsManager::GetBodyLockInterface()
{ assert(mSystem); return mSystem->GetBodyLockInterface(); }

BodyID& PhysicsManager::GetBodyID(ID uid)
{ return sBodyIDMap[uid]; }

bool PhysicsManager::DestroyBody(ID uid)
{
    if(!ValidateColliderUID(uid, true))
        { return false; }
    GetBodyInterface().RemoveBody(sBodyIDMap.at(uid));
    GetBodyInterface().DestroyBody(sBodyIDMap.at(uid));
    sBodyIDMap.erase(uid);
    return true;
}

static bool s_MakeSimpleBodySettings(BodyCreationSettings& outSettings, Farg<Transform3D> inTransform, PhysicsBodyShape inShape, EMotionType inMotion, ObjectLayer inLayer)
{
    RVec3 scale{GlmToJolt<Vec3>(inTransform.Scale())};
    RVec3 position{GlmToJolt<Vec3>(inTransform.Origin())};
    Quat rotation{JPH::Quat::sEulerAngles(GlmToJolt<Vec3>(inTransform.Euler()))};

    switch(inShape)
    {
    case PhysicsBodyShape::Box:
        outSettings = BodyCreationSettings(
            new BoxShapeSettings(scale),
            position,
            rotation,
            inMotion,
            inLayer);
        break;
    case PhysicsBodyShape::Sphere:
        outSettings = BodyCreationSettings(
            new SphereShapeSettings(scale.GetX() / 2.0f),
            position,
            rotation,
            inMotion,
            inLayer);
        break;
    case PhysicsBodyShape::Cylinder:
        outSettings = BodyCreationSettings(
            new CylinderShapeSettings(scale.GetZ() / 2.0f, scale.GetX() / 2.0f),
            position,
            rotation,
            inMotion,
            inLayer);
        break;
    case PhysicsBodyShape::Capsule:
        outSettings = BodyCreationSettings(
            new CapsuleShapeSettings(scale.GetZ() / 2.0f, scale.GetX() / 2.0f),
            position,
            rotation,
            inMotion,
            inLayer);
        break;
    default:
        return false;
    }
    return true;
}

static EMotionType s_GetMotion(PhysicsBodyMotion inMotion)
{
    switch(inMotion)
    {
    case PhysicsBodyMotion::Dynamic:
        return EMotionType::Dynamic;
    case PhysicsBodyMotion::Kinematic:
        return EMotionType::Kinematic;
    case PhysicsBodyMotion::None:
    default:
        print_warning("Invalid motion type detected; defaulting to `EMotionType::Static`");
    case PhysicsBodyMotion::Static:
        return EMotionType::Static;
    }
}

static ObjectLayer s_GetLayer(PhysicsBodyMotion inMotion)
{
    switch(inMotion)
    {
    case PhysicsBodyMotion::Dynamic:
        return Layers::MOVING;
    case PhysicsBodyMotion::Kinematic:
        return Layers::MOVING;
    case PhysicsBodyMotion::None:
    default:
        print_warning("Invalid motion type detected; defaulting to `Layers::NON_MOVING`");
    case PhysicsBodyMotion::Static:
        return Layers::NON_MOVING;
    }
}

bool PhysicsManager::CreateBody(ID uid,
    Farg<Transform3D> inTransform,
    PhysicsBodyShape inShape,
    PhysicsBodyMotion inMotion,
    float inFriction,
    float inDensity)
{
    if(!ValidateColliderUID(uid, false))
        { return false; }
    BodyCreationSettings settings;
    EMotionType motion_type{s_GetMotion(inMotion)};
    ObjectLayer layer{s_GetLayer(inMotion)};

    if(!s_MakeSimpleBodySettings(settings, inTransform, inShape, motion_type, layer))
    {
        return print_error("invalid physics body type '{}' from Collider#{}",
            EnumPrettifier::Get(inShape, EnumSet::PhysicsBodyShape),
            uid[]);
    }
#pragma message("TODO: Add bodies in a batch and activate them in a batch")
    if(inFriction)
        { settings.mFriction = inFriction; }
    if(inDensity)
    {
        settings.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
        settings.mMassPropertiesOverride.SetMassAndInertiaOfSolidBox(
            GlmToJolt<Vec3>(inTransform.Scale()),
            inDensity);
    }
    sBodyIDMap[uid] = GetBodyInterface().CreateAndAddBody(settings, EActivation::Activate);
    return !sBodyIDMap.at(uid).IsInvalid();
}

bool PhysicsManager::ValidateColliderUID(ID uid, bool check_body_id)
{
    if(!mSystem)
        { return print_error("Jolt physics system is nullptr!"); }
    else if(check_body_id && (!sBodyIDMap.contains(uid) || sBodyIDMap.at(uid).IsInvalid()))
        { return print_error("No valid BodyID paired with UID {}", uid[]); }
    return true;
}
