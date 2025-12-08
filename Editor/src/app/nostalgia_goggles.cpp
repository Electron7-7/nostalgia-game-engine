#include "nostalgia_goggles.hpp"
#include "application/window.hpp"
#include "events/event.hpp"
#include "managers/input_manager.hpp"
#include "gui/imgui_editor.hpp"
#include "gui/imgui_debugger.hpp"

std::string gToggleFullscreen{"ToggleFullscreen"};

void NostalgiaGoggles::Input(InputEvent* event)
{
    if(event->IsActive(gToggleFullscreen))
    {
        MainWindow().SetWindowMode((MainWindow().GetWindowMode() == IWindow::WINDOW_MODE_WINDOWED)
            ? IWindow::WINDOW_MODE_FULLSCREEN
            : IWindow::WINDOW_MODE_WINDOWED);
    }
    else if(event->IsJustPressed(Key::Q) && event->IsModifierActive(Key::Mod_Control))
        { Application()->Stop(); }
}

void NostalgiaGoggles::Stop()
{ IManager::Stop(); }

int NostalgiaGoggles::Main()
{
    ImGui_Editor::Activate();
    ImGui_Debugger::Activate();

    // g_pInputManager->SetInputEventCallback(sInputEventHandler);
    g_pInputManager->SetAction(InputAction{gToggleFullscreen, Key::F});
    g_pInputManager->SetAction(InputAction{"+forward",  Key::W});
    g_pInputManager->SetAction(InputAction{"+backward", Key::S});
    g_pInputManager->SetAction(InputAction{"+left",     Key::A});
    g_pInputManager->SetAction(InputAction{"+right",    Key::D});

    IManager::Start(); // gameloop

    ImGui_Debugger::Deactivate();
    ImGui_Editor::Deactivate();

    return 0;
}

void NostalgiaGoggles::Event(AppEvent* inEvent)
{
    print_debug("AppEvent");
    if(inEvent->IsEvent(AppEvents::WindowClose))
        { print_debug("WindowClose"); Stop(); }
}
