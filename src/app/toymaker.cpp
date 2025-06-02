#include "toymaker.hpp"
#include "manager.hpp"
#include "world_manager.hpp"
#include "render_manager.hpp"
#include "input_manager.hpp"

static TheToyMaker singleton_TheToyMakerApp;
TheToyMaker* global_App = &singleton_TheToyMakerApp;

bool TheToyMaker::Create()
{
    if(!BaseClass::Create())
        return false;

    // Add app systems
    // _System::Add(global_InputSystem);

    return true;
}

bool TheToyMaker::PreInit()
{
    if(!BaseClass::PreInit())
        return false;

    if(!global_BackendManager->Init())
        return false;

    // Check for missing interfaces...

    return true;
}

void TheToyMaker::Shutdown()
{
    _Manager::Stop();
}

void TheToyMaker::PostShutdown()
{}

int TheToyMaker::Main()
{
    // if(!SetVideoMode())
    //  return 0;

    // global_MaterialSystem->ModInit();

    // World database
    _Manager::Add(global_WorldManager);

    // Output
    _Manager::Add(global_RenderManager);

    // Input
    // _Manager::Add(global_NetworkManager);
    _Manager::Add(global_InputManager);
    // _Manager::Add(global_MenuManager);
    // _Manager::Add(global_UIManager);

    // Physics
    // _Manager::Add(global_PhysicsManager);

    // Init the systems
    // if(!_System::InitAllSystems())
        // return 0;

    // Start the main system loop
    // _System::Start();

    // Init the managers
    if(!_Manager::InitAllManagers())
        return 0;

    // First menu to start on
    // global_MenuManager->PushMenu("Main Menu");

    // This is the main game loop
    _Manager::Start();

    // Shut down game systems after main loop ends
    _Manager::ShutdownAllManagers();

    // global_MaterialSystem->ModShutdown();

    // Note: I'm assuming that the last function called would exit the program and this is if that function fails? Idk, so I'm gonna just write 'return 0'
    // return 1;

    return 0;
}