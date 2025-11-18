#include "nostalgia_goggles.hpp"
#include "ui/imgui_debugger.hpp"
#include "ui/main_window.hpp"
#include "application/window.hpp"
#include "input/event.hpp"
#include "managers/input_manager.hpp"
#include "managers/ui_manager.hpp"

std::string gToggleFullscreen{"ToggleFullscreen"};

static SafeStatus sInputEventHandler(const InputEvent& event)
{
    if(event.IsActionPressed(gToggleFullscreen))
    {
        g_pApplication->GetWindow().SetWindowMode(
            (g_pApplication->GetWindow().GetWindowMode() == IWindow::WINDOW_MODE_WINDOWED)
                ? IWindow::WINDOW_MODE_FULLSCREEN
                : IWindow::WINDOW_MODE_WINDOWED);
    }
    else if(event.IsKeyDown(BindingID::KeyLEFTCONTROL) && event.IsKeyPressed(BindingID::KeyQ))
        { g_pApplication->Stop(); }
    return Status::NO_ERR;
}

void NostalgiaGoggles::Stop()
{ IManager::Stop(); }

int NostalgiaGoggles::Main()
{
    g_pInputManager->SetInputEventCallback(sInputEventHandler);

    // g_pMenuManager->PushMenu("Main Menu");
    g_pUIManager->AddImGuiObject(g_pDebugger);
    g_pUIManager->AddImGuiObject(g_pMainWindow);

    g_pInputManager->AssignAction(gToggleFullscreen, BindingID::KeyF);
    g_pInputManager->AssignAction("+forward",  BindingID::KeyW);
    g_pInputManager->AssignAction("+backward", BindingID::KeyS);
    g_pInputManager->AssignAction("+left",     BindingID::KeyA);
    g_pInputManager->AssignAction("+right",    BindingID::KeyD);

    IManager::Start(); // gameloop
    return 0;
}
