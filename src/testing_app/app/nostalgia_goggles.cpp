#include "nostalgia_goggles.hpp"
#include "managers/manager.hpp"
#include "managers/physics_manager.hpp"
#include "managers/theatre_manager.hpp"
#include "managers/backend_manager.hpp"
#include "managers/render_manager.hpp"
#include "managers/input_manager.hpp"
#include "managers/ui_manager.hpp"
#include "../ui/imgui_debugger.hpp"

static NostalgiaGoggles s_NostalgiaGogglesApp;
NostalgiaGoggles* g_pApplication = &s_NostalgiaGogglesApp;

static std::string sToggleFullscreen   ("ToggleFullscreen");
static std::string sToggleMouseCapture ("ToggleMouseCapture");

static SafeStatus sInputEventHandler(const InputEvent& event)
{
    if(event.IsInput(sToggleFullscreen) && event.JustPressed())
        { g_pBackendManager->Windowing()->ToggleFullscreen(); }
    else if(event.IsInput(sToggleMouseCapture) && event.JustPressed())
        { g_pBackendManager->Windowing()->ToggleMouseMode(MouseMode::Disabled); }
    return Status::NO_ERR;
}

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
{ _Manager::Stop(); }

void NostalgiaGoggles::PostShutdown()
{
    _Manager::ShutdownAllManagers();
    _Manager::RemoveAll();
}

int NostalgiaGoggles::Main()
{
    _Manager::Add(g_pPhysicsManager);
    _Manager::Add(g_pTheatreManager);
    _Manager::Add(g_pBackendManager);
    _Manager::Add(g_pRenderManager);
    _Manager::Add(g_pInputManager);
    _Manager::Add(g_pUIManager);
    // _Manager::Add(g_pMenuManager);

    if(!_Manager::InitAllManagers())
    {
        Shutdown();
        print_error("NostalgiaGoggles::Main - _Manager::InitAllManagers failed!!");
        return 1;
    }

    g_pInputManager->SetInputEventCallback(sInputEventHandler);

    // g_pMenuManager->PushMenu("Main Menu");
    g_pUIManager->AddImGuiObject(g_pDebugger);

    InputManager::AddAction(sToggleFullscreen,   BindingIDs::KeyF);
    InputManager::AddAction(sToggleMouseCapture, BindingIDs::KeyESC);
    InputManager::AddAction("+forward",  BindingIDs::KeyW);
    InputManager::AddAction("+backward", BindingIDs::KeyS);
    InputManager::AddAction("+left",     BindingIDs::KeyA);
    InputManager::AddAction("+right",    BindingIDs::KeyD);

    _Manager::Start(); // gameloop
    return 0;
}
