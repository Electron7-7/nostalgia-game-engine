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
    EnumRegistry::Assign(MotionType::Static,    "Static");
    EnumRegistry::Assign(MotionType::Dynamic,   "Dynamic");
    EnumRegistry::Assign(MotionType::Kinematic, "Kinematic");
    EnumRegistry::Assign(ShapeType::Box,        "Box");
    EnumRegistry::Assign(ShapeType::Capsule,    "Capsule");
    EnumRegistry::Assign(ShapeType::Cylinder,   "Cylinder");
    EnumRegistry::Assign(ShapeType::Sphere,     "Sphere");
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
