#include "nostalgia_goggles.hpp"
#include "application/window.hpp"
#include "events/event_queue.hpp"
#include "events/action.hpp"
#include "managers/physics_manager.hpp"
#include "managers/theatre_manager.hpp"
#include "managers/render_manager.hpp"
#include "managers/input_manager.hpp"
#include "managers/event_manager.hpp"
#include "managers/ui_manager.hpp"
#include "ui/implementor.hpp"
#include "ui/solution.hpp"
#include "gui/imgui_editor.hpp"
#include "gui/imgui_debugger.hpp"
#include "backends/glfw/glfw_window.hpp" // GLFW is the main windowing solution (most likely permanently)
#include "backends/imgui/imgui_implementor.hpp"
#include <thread>

static ImGui_Editor sImGui_Editor{};
static ImGui_Debugger sImGui_Debugger{};

std::string gToggleFullscreen{"ToggleFullscreen"};

void NostalgiaGoggles::Stop()
{ IManager::Stop(); }

int NostalgiaGoggles::Main()
{
    mMainWindow = IWindow::CreateWindow<WindowGLFW>(); // Using the default `WindowProperties` constructor

    auto& imgui_impl{UI_Implementor::Create<ImGui_Implementor>()};
    imgui_impl->CreateSolution<ImGui_Editor>();
    imgui_impl->CreateSolution<ImGui_Debugger>();

    IManager::Add(g_pPhysicsManager);
    IManager::Add(g_pTheatreManager);
    IManager::Add(g_pRenderManager);
    IManager::Add(g_pInputManager);
    IManager::Add(g_pEventManager);
    IManager::Add(g_pUIManager);

    IManager::InitAllManagers();

    ApplicationEventsHandler* handler{new ApplicationEventsHandler};

    g_pInputManager->SetAction(InputAction{gToggleFullscreen, Key::F});
    g_pInputManager->SetAction(InputAction{"+forward",  Key::W});
    g_pInputManager->SetAction(InputAction{"+backward", Key::S});
    g_pInputManager->SetAction(InputAction{"+left",     Key::A});
    g_pInputManager->SetAction(InputAction{"+right",    Key::D});

    IManager::Start(); // gameloop
    IManager::ShutdownAllManagers();
    IManager::RemoveAll();

    delete handler;

    return 0;
}

void ApplicationEventsHandler::Event(AppEvent* inEvent)
{
    if(inEvent->IsEvent(AppEvent::WindowClose))
        { Application()->Stop(); }
}

void ApplicationEventsHandler::Input(InputEvent* event)
{
    if(event->IsActive(gToggleFullscreen))
    {
        MainWindow()->SetWindowMode((MainWindow()->GetWindowMode() == IWindow::WINDOW_MODE_WINDOWED)
            ? IWindow::WINDOW_MODE_FULLSCREEN
            : IWindow::WINDOW_MODE_WINDOWED);
    }
    else if((event->IsJustPressed(Key::Q) and event->IsModifierActive(Key::Mod_Control))
        or event->IsJustPressed(Key::F8))
        { EventManager::Queue()->add<AppEvent>(AppEvent::WindowClose); }
}
