#include "nostalgia_goggles.hpp"
#include "editor/ui/imgui_debugger.hpp"
#include "editor/ui/main_window.hpp"
#include "settings/window.hpp"
#include "managers/manager.hpp"
#include "managers/physics_manager.hpp"
#include "managers/theatre_manager.hpp"
#include "managers/backend_manager.hpp"
#include "managers/render_manager.hpp"
#include "managers/input_manager.hpp"
#include "managers/ui_manager.hpp"

static NostalgiaGoggles s_NostalgiaGogglesApp;
NostalgiaGoggles* g_pApplication = &s_NostalgiaGogglesApp;

std::string gToggleFullscreen{"ToggleFullscreen"};

static SafeStatus sInputEventHandler(const InputEvent& event)
{
    if(event.IsActionPressed(gToggleFullscreen))
        { Settings::Window::ToggleFullscreen(); }
    else if(event.IsKeyDown(BindingIDs::KeyLEFTCONTROL) && event.IsKeyPressed(BindingIDs::KeyQ))
        { g_pApplication->Shutdown(); }
    return Status::NO_ERR;
}

bool NostalgiaGoggles::Create()
{
    if(!IApplication::Create())
        { return false; }
    return true;
}

bool NostalgiaGoggles::PreInit()
{
    if(!IApplication::PreInit())
        { return false; }
    return true;
}

void NostalgiaGoggles::Shutdown()
{ IManager::Stop(); }

void NostalgiaGoggles::PostShutdown()
{
    IManager::ShutdownAllManagers();
    IManager::RemoveAll();
}

int NostalgiaGoggles::Main()
{
    IManager::Add(g_pPhysicsManager);
    IManager::Add(g_pTheatreManager);
    IManager::Add(g_pBackendManager);
    IManager::Add(g_pRenderManager);
    IManager::Add(g_pInputManager);
    IManager::Add(g_pUIManager);
    // IManager::Add(g_pMenuManager);

    if(!IManager::InitAllManagers())
    {
        Shutdown();
        return !print_error("NostalgiaGoggles::Main - IManager::InitAllManagers failed!!");
    }

    g_pInputManager->SetInputEventCallback(sInputEventHandler);

    // g_pMenuManager->PushMenu("Main Menu");
    g_pUIManager->AddImGuiObject(g_pDebugger);
    g_pUIManager->AddImGuiObject(g_pMainWindow);

    g_pInputManager->AssignAction(gToggleFullscreen, BindingIDs::KeyF);
    g_pInputManager->AssignAction("+forward",  BindingIDs::KeyW);
    g_pInputManager->AssignAction("+backward", BindingIDs::KeyS);
    g_pInputManager->AssignAction("+left",     BindingIDs::KeyA);
    g_pInputManager->AssignAction("+right",    BindingIDs::KeyD);

    IManager::Start(); // gameloop
    return 0;
}
