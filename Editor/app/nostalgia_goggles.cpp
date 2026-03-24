#include "./nostalgia_goggles.hpp"
#include "gui/imgui_implementor.hpp"
#include "gui/imgui_editor.hpp"
#include "gui/imgui_debugger.hpp"
#include "things/player.hpp"
#include "things/test_animated_sprite.hpp"
#include <Nostalgia/application/window.hpp>
#include <Nostalgia/events/event_queue.hpp>
#include <Nostalgia/events/action.hpp>
#include <Nostalgia/managers/physics_manager.hpp>
#include <Nostalgia/managers/theatre_manager.hpp>
#include <Nostalgia/managers/render_manager.hpp>
#include <Nostalgia/managers/input_manager.hpp>
#include <Nostalgia/managers/event_manager.hpp>
#include <Nostalgia/managers/ui_manager.hpp>
#include <Nostalgia/settings/engine.hpp>
#include <Nostalgia/ui/implementor.hpp>
#include <Nostalgia/things/thing_factory.hpp>

static ImGui_Editor sImGui_Editor{};
static ImGui_Debugger sImGui_Debugger{};

std::string gToggleFullscreen{"ToggleFullscreen"};

void NostalgiaGoggles::Stop()
{ IManager::Stop(); }

int NostalgiaGoggles::Main()
{
    mMainWindow = IWindow::CreateNewWindow(IWindow::Properties{std::format("Nostalgia_Goggles v{} {}",
        NOSTALGIA_VERSION_STRING,
        (Settings::Engine::IsEditorHint)
            ? ""
            : "(Settings::Engine::IsEditorHint == false)")});

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

    ThingFactory::AddThing(&ThingFactory::ThingMakerTemplate<EditorPlayer3D>, "EditorPlayer3D", ThingType::NostalgiaPlayer3D);
    ThingFactory::AddThing(&ThingFactory::ThingMakerTemplate<TestAnimatedSprite2D>, "TestAnimatedSprite2D", ThingType::Sprite2D);

    g_pInputManager->SetAction({gToggleFullscreen, Key::F10});
    g_pInputManager->SetAction({"+forward",  Key::W});
    g_pInputManager->SetAction({"+backward", Key::S});
    g_pInputManager->SetAction({"+left",     Key::A});
    g_pInputManager->SetAction({"+right",    Key::D});

    IManager::Start(); // gameloop
    IManager::ShutdownAllManagers();
    IManager::RemoveAll();

    return 0;
}

void NostalgiaGoggles::Event(AppEvent* inEvent)
{
    if(inEvent->IsEvent(AppEvent::WindowClose))
        { Application()->Stop(); }
}

void NostalgiaGoggles::Input(InputEvent* event)
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
