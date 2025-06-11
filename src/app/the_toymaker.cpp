#include "the_toymaker.hpp"
#include "engine/managers/manager.hpp"
#include "engine/managers/input_manager.hpp"
#include "engine/managers/world_manager.hpp"
#include "engine/managers/render_manager.hpp"

// Singleton accessor
TheToymaker singleton_TheToymakerApp;
TheToymaker* global_EditorApp = &singleton_TheToymakerApp;

bool TheToymaker::Create()
{
    if(!BaseClass::Create())
        return false;

    return true;
}

bool TheToymaker::PreInit()
{
    if(!BaseClass::PreInit())
        return false;

    return true;
}

void TheToymaker::Shutdown()
{
    // NOTE: This might not be safe/enough
    _Manager::Stop();
    _Manager::ShutdownAllManagers();
    _Manager::RemoveAll();
}

void TheToymaker::PostShutdown()
{}

int TheToymaker::Main()
{
    // World database
    _Manager::Add(global_WorldManager);

    // Output
    _Manager::Add(global_RenderManager);

    // Input
    _Manager::Add(global_InputManager);
    // _Manager::Add(global_MenuManager);
    // _Manager::Add(global_UIManager);

    // Physics
    // _Manager::Add(global_PhysicsManager);

    // Init the managers
    if(!_Manager::InitAllManagers())
        return 0;

    // First menu to start on
    // global_MenuManager->PushMenu("Main Menu");

    // This is the main game loop
    _Manager::Start();

    // Shut down game systems after main loop ends
    _Manager::ShutdownAllManagers();

    return 0;
}

