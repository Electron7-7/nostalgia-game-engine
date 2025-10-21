#include "nostalgia_goggles.hpp"
#include "../ui/imgui_debugger.hpp"
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
std::string gToggleMouseCapture{"ToggleMouseCapture"};
static std::string sExitApplication{"ExitApplication"};

static SafeStatus sInputEventHandler(const InputEvent& event)
{
    if(event.IsAction(gToggleFullscreen) && event.JustPressed())
        { g_pBackendManager->Windowing()->ToggleFullscreen(); }
    else if(event.IsAction(gToggleMouseCapture) && event.JustPressed())
        { g_pBackendManager->Windowing()->ToggleMouseMode(MouseMode::Disabled); }
    else if(event.IsAction(sExitApplication) && event.JustPressed())
        { g_pApplication->Shutdown(); }
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
        return !print_error("NostalgiaGoggles::Main - _Manager::InitAllManagers failed!!");
    }

    g_pInputManager->SetInputEventCallback(sInputEventHandler);

    // g_pMenuManager->PushMenu("Main Menu");
    g_pUIManager->AddImGuiObject(g_pDebugger);

    g_pInputManager->AssignAction(gToggleFullscreen,   BindingIDs::KeyF);
    g_pInputManager->AssignAction(gToggleMouseCapture, BindingIDs::KeyESC);
    g_pInputManager->AssignAction("+forward",  BindingIDs::KeyW);
    g_pInputManager->AssignAction("+backward", BindingIDs::KeyS);
    g_pInputManager->AssignAction("+left",     BindingIDs::KeyA);
    g_pInputManager->AssignAction("+right",    BindingIDs::KeyD);

    _Manager::Start(); // gameloop
    return 0;
}
