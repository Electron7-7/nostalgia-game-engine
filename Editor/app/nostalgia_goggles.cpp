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
#include <Nostalgia/managers/resource_manager.hpp>
#include <Nostalgia/managers/theatre_manager.hpp>
#include <Nostalgia/managers/render_manager.hpp>
#include <Nostalgia/managers/input_manager.hpp>
#include <Nostalgia/managers/event_manager.hpp>
#include <Nostalgia/managers/ui_manager.hpp>
#include <Nostalgia/settings/engine.hpp>
#include <Nostalgia/ui/implementor.hpp>
#include <Nostalgia/things/thing_factory.hpp>
#include <Nostalgia/rendering/mesh_buffers.hpp>

static ImGui_Editor sImGui_Editor{};
static ImGui_Debugger sImGui_Debugger{};

std::string gToggleFullscreen{"ToggleFullscreen"};

void NostalgiaGoggles::Stop()
{ Manager::Stop(); }

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

    Manager::Add(g_pRenderManager);
    Manager::Add(g_pResourceManager);
    Manager::Add(g_pPhysicsManager);
    Manager::Add(g_pTheatreManager);
    Manager::Add(g_pInputManager);
    Manager::Add(g_pEventManager);
    Manager::Add(g_pUIManager);

    Manager::InitAllManagers();

    ThingFactory::AddThing(&ThingFactory::ThingMakerTemplate<EditorPlayer3D>, "EditorPlayer3D", ThingType::NostalgiaPlayer3D);
    ThingFactory::AddThing(&ThingFactory::ThingMakerTemplate<TestAnimatedSprite2D>, "TestAnimatedSprite2D", ThingType::Sprite2D);

    EnumRegistry::Assign(IBuffer::Element::Type::None,   "None");
    EnumRegistry::Assign(IBuffer::Element::Type::Float,  "Float");
    EnumRegistry::Assign(IBuffer::Element::Type::Float2, "Float2");
    EnumRegistry::Assign(IBuffer::Element::Type::Float3, "Float3");
    EnumRegistry::Assign(IBuffer::Element::Type::Float4, "Float4");
    EnumRegistry::Assign(IBuffer::Element::Type::Int,    "Int");
    EnumRegistry::Assign(IBuffer::Element::Type::Int2,   "Int2");
    EnumRegistry::Assign(IBuffer::Element::Type::Int3,   "Int3");
    EnumRegistry::Assign(IBuffer::Element::Type::Int4,   "Int4");
    EnumRegistry::Assign(IBuffer::Element::Type::Mat3,   "Mat3");
    EnumRegistry::Assign(IBuffer::Element::Type::Mat4,   "Mat4");
    EnumRegistry::Assign(IBuffer::Element::Type::Bool,   "Bool");

    g_pInputManager->SetAction({gToggleFullscreen, Key::F10});
    g_pInputManager->SetAction({"+forward",  Key::W});
    g_pInputManager->SetAction({"+backward", Key::S});
    g_pInputManager->SetAction({"+left",     Key::A});
    g_pInputManager->SetAction({"+right",    Key::D});

    Manager::Start(); // gameloop
    Manager::ShutdownAllManagers();
    Manager::RemoveAll();

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
