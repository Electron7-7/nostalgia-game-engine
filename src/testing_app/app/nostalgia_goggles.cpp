#include "nostalgia_goggles.hpp"
#include "managers/manager.hpp"
#include "managers/input_manager.hpp"
#include "managers/world_manager.hpp"
#include "managers/render_manager.hpp"
#include "managers/ui_manager.hpp"
#include "testing_app/ui/prototype_debug_window.hpp"

// Singleton accessor
NostalgiaGoggles singleton_NostalgiaGogglesApp;
NostalgiaGoggles* global_Application = &singleton_NostalgiaGogglesApp;

bool NostalgiaGoggles::Create()
{
    if(!BaseClass::Create())
        return false;

    return true;
}

bool NostalgiaGoggles::PreInit()
{
    if(!BaseClass::PreInit())
        return false;

    return true;
}

void NostalgiaGoggles::Shutdown()
{
    // NOTE: This might not be safe/enough
    _Manager::Stop();
    _Manager::ShutdownAllManagers();
    _Manager::RemoveAll();
}

void NostalgiaGoggles::PostShutdown()
{}

int NostalgiaGoggles::Main()
{
    // World database
    _Manager::Add(global_WorldManager);

    // Output
    _Manager::Add(global_RenderManager);

    // Input
    _Manager::Add(global_InputManager);
    // _Manager::Add(global_MenuManager);
    _Manager::Add(global_UIManager);

    global_UIManager->AddImGuiObject(global_DebugWindow);

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
