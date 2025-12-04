#include "nostalgia_goggles.hpp"
#include "application/window.hpp"
#include "events/event.hpp"
#include "managers/event_manager.hpp"
#include "gui/imgui_editor.hpp"
#include "gui/imgui_debugger.hpp"

std::string gToggleFullscreen{"ToggleFullscreen"};

static void sInputEventHandler(InputEvent* event)
{
    if(event->IsActive(gToggleFullscreen))
    {
        MainWindow().SetWindowMode((MainWindow().GetWindowMode() == IWindow::WINDOW_MODE_WINDOWED)
            ? IWindow::WINDOW_MODE_FULLSCREEN
            : IWindow::WINDOW_MODE_WINDOWED);
    }
    else if(event->IsJustPressed(Key::Q) && event->IsModifierActive(Key::Modifier::Control))
        { Application()->Stop(); }
}

void NostalgiaGoggles::Stop()
{ IManager::Stop(); }

int NostalgiaGoggles::Main()
{
    ImGui_Editor::Activate();
    ImGui_Debugger::Activate();

    g_pEventManager->SetInputEventCallback(sInputEventHandler);
    g_pEventManager->SetAction(InputAction{gToggleFullscreen, Key::F});
    g_pEventManager->SetAction(InputAction{"+forward",  Key::W});
    g_pEventManager->SetAction(InputAction{"+backward", Key::S});
    g_pEventManager->SetAction(InputAction{"+left",     Key::A});
    g_pEventManager->SetAction(InputAction{"+right",    Key::D});

    IManager::Start(); // gameloop

    ImGui_Debugger::Deactivate();
    ImGui_Editor::Deactivate();

    return 0;
}
