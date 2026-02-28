#include "physics_manager.hpp"
#include "settings/engine.hpp"
#include "physics/engine.hpp"

static PhysicsManager sPhysicsManager;
PhysicsManager* g_pPhysicsManager{&sPhysicsManager};

// TODO: Read JoltPhysics `HelloWorld.cpp` and make the necessary changes as you implement more of the engine
bool PhysicsManager::Init()
{
    PRINT_PRETTY_FUNCTION;
    PhysicsEngine::Instantiate();
    return PhysicsEngine::Instance() != nullptr;
}

void PhysicsManager::Shutdown() {}

ManagerEnums::TheatreReturnValue_t PhysicsManager::TheatreInit(bool is_first_call)
{
    PRINT_PRETTY_FUNCTION_EXT("BEGIN");
    if(!is_first_call)
        { return ManagerEnums::FINISHED; }
    PhysicsEngine::Instance()->Start();
    return ManagerEnums::FINISHED;
    PRINT_PRETTY_FUNCTION_EXT("END");
}

ManagerEnums::TheatreReturnValue_t PhysicsManager::TheatreShutdown(bool is_first_call)
{
    PRINT_PRETTY_FUNCTION_EXT("BEGIN");
    if(!is_first_call)
        { return ManagerEnums::FINISHED; }
    PhysicsEngine::Instance()->Stop();
    return ManagerEnums::FINISHED;
    PRINT_PRETTY_FUNCTION_EXT("END");
}

void PhysicsManager::Tick()
{
    if(GetTheatreState() != ManagerEnums::IN_LEVEL)
        { return; }
    PhysicsEngine::Instance()->Tick(Settings::Engine::TickInterval());
}
