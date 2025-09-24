#include "nostalgia_goggles.hpp"
#include "input/keybind.hpp"
#include "input/event_queue.hpp"
#include "managers/manager.hpp"
#include "managers/theatre_manager.hpp"
#include "managers/backend_manager.hpp"
#include "managers/render_manager.hpp"
#include "managers/input_manager.hpp"
#include "managers/ui_manager.hpp"
#include "commands/command_line.hpp"
#include "testing_app/ui/imgui_debugger.hpp"

// Singleton accessor
static NostalgiaGoggles s_NostalgiaGogglesApp;
NostalgiaGoggles* g_pApplication = &s_NostalgiaGogglesApp;

bool NostalgiaGoggles::Create()
{
    if(!BaseClass::Create())
    { return false; }

    return true;
}

bool NostalgiaGoggles::PreInit()
{
    if(!BaseClass::PreInit())
    { return false; }

    return true;
}

void NostalgiaGoggles::Shutdown()
{
    _Manager::RemoveAll();
}

void NostalgiaGoggles::PostShutdown()
{}

int NostalgiaGoggles::Main()
{
    _Manager::Add(g_pTheatreManager);
    _Manager::Add(g_pBackendManager);
    _Manager::Add(g_pRenderManager);
    _Manager::Add(g_pInputManager);
    _Manager::Add(g_pUIManager);
    // _Manager::Add(g_pMenuManager);
    // _Manager::Add(g_pPhysicsManager);

    if(!_Manager::InitAllManagers())
        { return 1; }

    // g_pMenuManager->PushMenu("Main Menu");
    g_pUIManager->AddImGuiObject(g_pDebugger);

    try_AddBinding("Escape", CommandLine::cmd_ExitProgram);
    try_AddBinding("F", CommandLine::cmd_PrototypeFullscreen);
    EventQueue::EnableEventQueue();

    _Manager::Start(); // gameloop
    _Manager::ShutdownAllManagers();
    return 0;
}
