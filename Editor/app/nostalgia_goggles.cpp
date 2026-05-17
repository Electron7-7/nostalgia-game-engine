#include "./nostalgia_goggles.hpp"
#include "gui/imgui_implementor.hpp"
#include "gui/imgui_editor.hpp"
#include "gui/imgui_debugger.hpp"
#include "things/editor_player_3d.hpp"
#include "things/test_animated_sprite.hpp"
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
#include <Nostalgia/things/thing_factory.hpp>
#include <Nostalgia/things/resources/image_texture.hpp>
#include <Nostalgia/things/resources/array_mesh.hpp>
#include <Nostalgia/rendering/buffers.hpp>

// Use a separate window title if in debug mode
#ifdef NOSTALGIA_DEBUGGING
#   define NOSTALGIA_TITLE_STRING "(DEBUG) NostalgiaGoggles v" NOSTALGIA_VERSION_STRING
#else
#   define NOSTALGIA_TITLE_STRING "NostalgiaGoggles v" NOSTALGIA_VERSION_STRING
#endif // NOSTALGIA_DEBUGGING

// Suppress LeakSanitizer (due to random memory leaks likely caused by libGL.so or X11/Wayland drivers)
// https://stackoverflow.com/a/51061314
#ifdef __cplusplus
extern "C"
#endif // __cplusplus
const char* __asan_default_options() { return "detect_leaks=0"; }

static ImGui_Editor sImGui_Editor{};
static ImGui_Debugger sImGui_Debugger{};

std::string gToggleFullscreen{"ToggleFullscreen"};

void NostalgiaGoggles::Stop()
{ Manager::Stop(); }

int NostalgiaGoggles::Main()
{
    m_pMainWindow = IWindow::CreateNewWindow({NOSTALGIA_TITLE_STRING, m_sMainWindowWidth, m_sMainWindowHeight});

    Console::Init();
    Console::SetVariable("ThingFactory.debug_type_msgs", m_sEnableThingFactoryDebugMsgs);

    m_pUII = UI_Implementor::Create<ImGui_Implementor>();
    m_pEditor = m_pUII->CreateSolution<ImGui_Editor>();
    m_pUII->CreateSolution<ImGui_Debugger>();

    m_pEditor->SetStartupTheatre(m_sStartupTheatre);

    IManager::FPSCounter(DEBUG_BUILD);
    g_pRenderManager->CalculateFrameTime(DEBUG_BUILD);

    Manager::Add(g_pRenderManager);
    Manager::Add(g_pResourceManager);
    Manager::Add(g_pPhysicsManager);
    Manager::Add(g_pTheatreManager);
    Manager::Add(g_pInputManager);
    Manager::Add(g_pEventManager);
    Manager::Add(g_pUIManager);

    Manager::InitAllManagers();

    Console::SetVariable("ThingFactory.debug_type_msgs", DEBUG_BUILD);

    ThingFactory::AddThingType(&ThingFactory::ThingMakerTemplate<EditorPlayer3D>,
        "EditorPlayer3D",
        ThingType::Collider3D);
    ThingFactory::AddThingType(&ThingFactory::ThingMakerTemplate<TestAnimatedSprite2D>,
        "TestAnimatedSprite2D",
        ThingType::Sprite2D);

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

void NostalgiaGoggles::Event(IEvent* inEvent)
{
    if(auto _event{static_cast<WindowEvent*>(inEvent)}; _event
        and _event->IsEvent(WindowEvent::WindowClose)
        and _event->IsMainWindow())
    {
        if(m_pEditor)
            { m_pEditor->QuitEditor(); }
        else
            { Stop(); }
    }
}

void NostalgiaGoggles::Input(InputEvent* event)
{
    if(event->IsActive(gToggleFullscreen))
    {
        MainWindow()->SetWindowMode((MainWindow()->GetWindowMode() == IWindow::WINDOW_MODE_WINDOWED)
            ? IWindow::WINDOW_MODE_FULLSCREEN
            : IWindow::WINDOW_MODE_WINDOWED);
    }
}

const IWindow* NostalgiaGoggles::MainWindow() const
{ return m_pMainWindow.get(); }

IWindow* NostalgiaGoggles::MainWindow()
{ return m_pMainWindow.get(); }
