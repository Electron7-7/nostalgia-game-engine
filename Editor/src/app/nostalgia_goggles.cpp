#include "nostalgia_goggles.hpp"
#include "application/window.hpp"
#include "events/event.hpp"
#include "managers/event_manager"
#include "gui/imgui_editor.hpp"
#include "gui/imgui_debugger.hpp"

std::string gToggleFullscreen{"ToggleFullscreen"};

static void sInputEventHandler(const InputEvent& event)
{
    if(event.IsActionPressed(gToggleFullscreen))
    {
        MainWindow().SetWindowMode((MainWindow().GetWindowMode() == IWindow::WINDOW_MODE_WINDOWED)
            ? IWindow::WINDOW_MODE_FULLSCREEN
            : IWindow::WINDOW_MODE_WINDOWED);
    }
    else if(event.IsKeyDown(BindingID::KeyLEFTCONTROL) && event.IsKeyPressed(BindingID::KeyQ))
        { Application()->Stop(); }
}

void NostalgiaGoggles::Stop()
{ IManager::Stop(); }

int NostalgiaGoggles::Main()
{
    ImGui_Editor::Activate();
    ImGui_Debugger::Activate();

    g_pEventManager->SetInputEventCallback(sInputEventHandler);
    g_pEventManager->AssignAction(gToggleFullscreen, BindingID::KeyF);
    g_pEventManager->AssignAction("+forward",  BindingID::KeyW);
    g_pEventManager->AssignAction("+backward", BindingID::KeyS);
    g_pEventManager->AssignAction("+left",     BindingID::KeyA);
    g_pEventManager->AssignAction("+right",    BindingID::KeyD);

    IManager::Start(); // gameloop

    ImGui_Debugger::Deactivate();
    ImGui_Editor::Deactivate();

    return 0;
}
