#include "physics_manager.hpp"
#include "settings/engine.hpp"
#include "physics/engine.hpp"
#include "things/thinkers/3d/collider_3d.hpp"

static PhysicsManager sPhysicsManager;
PhysicsManager* g_pPhysicsManager{&sPhysicsManager};

// TODO: Read JoltPhysics `HelloWorld.cpp` and make the necessary changes as you implement more of the engine
bool PhysicsManager::Init()
{
    PRINT_PRETTY_FUNCTION;
    PhysicsEngine::Instantiate();
    EnumRegistry::Register(MotionType::Static,    "Motion::Static");
    EnumRegistry::Register(MotionType::Dynamic,   "Motion::Dynamic");
    EnumRegistry::Register(MotionType::Kinematic, "Motion::Kinematic");
    EnumRegistry::Register(MotionType::None,      "Motion::None");
    EnumRegistry::Register(ShapeType::Box,        "Shape::Box");
    EnumRegistry::Register(ShapeType::Capsule,    "Shape::Capsule");
    EnumRegistry::Register(ShapeType::Cylinder,   "Shape::Cylinder");
    EnumRegistry::Register(ShapeType::Sphere,     "Shape::Sphere");
    EnumRegistry::Register(ShapeType::None,       "Shape::None");
    return PhysicsEngine::Instance() != nullptr;
}

void PhysicsManager::Shutdown() {}

ManagerEnums::TheatreReturnValue_t PhysicsManager::TheatreInit(bool is_first_call)
{
    if(!is_first_call)
        { return ManagerEnums::FINISHED; }
    PhysicsEngine::Instance()->Start();
    return ManagerEnums::FINISHED;
}

ManagerEnums::TheatreReturnValue_t PhysicsManager::TheatreShutdown(bool is_first_call)
{
    if(!is_first_call)
        { return ManagerEnums::FINISHED; }
    PhysicsEngine::Instance()->Stop();
    return ManagerEnums::FINISHED;
}

void PhysicsManager::Tick()
{
    if(GetTheatreState() != ManagerEnums::IN_LEVEL)
        { return; }
    PhysicsEngine::Instance()->Tick(Settings::Engine::TickInterval());
}
