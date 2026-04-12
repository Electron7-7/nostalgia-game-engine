#include "./nostalgia_goggles.hpp"
#include "editor_icons.hpp"
#include "assets/icon_uids.hpp"
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
#include <Nostalgia/things/resource_database.hpp>
#include <Nostalgia/things/resources/image_texture.hpp>
#include <Nostalgia/rendering/mesh_buffers.hpp>

#define REGISTER_ICON(NAME, VAR_NAME, UID_OUT) \
    UID_OUT = ResourceDatabase::Register(ImageTexture::CreateFromImage(Image::CreateFromData(VAR_NAME, std::size(VAR_NAME))), #NAME);

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

    REGISTER_ICON(Actor2d, _EditorIcons::actor_2d, Icons::actor_2d)
    REGISTER_ICON(Actor3d, _EditorIcons::actor_3d, Icons::actor_3d)
    REGISTER_ICON(ArrayMesh, _EditorIcons::array_mesh, Icons::array_mesh)
    REGISTER_ICON(Camera2d, _EditorIcons::camera_2d, Icons::camera_2d)
    REGISTER_ICON(Camera3d, _EditorIcons::camera_3d, Icons::camera_3d)
    REGISTER_ICON(Collider3d, _EditorIcons::collider_3d, Icons::collider_3d)
    REGISTER_ICON(Cubemap, _EditorIcons::cubemap, Icons::cubemap)
    REGISTER_ICON(DirectionalLight3d, _EditorIcons::directional_light_3d, Icons::directional_light_3d)
    REGISTER_ICON(Font, _EditorIcons::font, Icons::font)
    REGISTER_ICON(Image, _EditorIcons::image, Icons::image)
    REGISTER_ICON(ImageTexture, _EditorIcons::image_texture, Icons::image_texture)
    REGISTER_ICON(Light3d, _EditorIcons::light_3d, Icons::light_3d)
    REGISTER_ICON(Material, _EditorIcons::material, Icons::material)
    REGISTER_ICON(Mesh, _EditorIcons::mesh, Icons::mesh)
    REGISTER_ICON(MeshInstance3d, _EditorIcons::mesh_instance_3d, Icons::mesh_instance_3d)
    REGISTER_ICON(NostalgiaPlayer3d, _EditorIcons::nostalgia_player_3d, Icons::nostalgia_player_3d)
    REGISTER_ICON(PointLight3d, _EditorIcons::point_light_3d, Icons::point_light_3d)
    REGISTER_ICON(Resource, _EditorIcons::resource, Icons::resource)
    REGISTER_ICON(SpotLight3d, _EditorIcons::spot_light_3d, Icons::spot_light_3d)
    REGISTER_ICON(Sprite2d, _EditorIcons::sprite_2d, Icons::sprite_2d)
    REGISTER_ICON(Sprite3d, _EditorIcons::sprite_3d, Icons::sprite_3d)
    REGISTER_ICON(Text2d, _EditorIcons::text_2d, Icons::text_2d)
    REGISTER_ICON(Texture, _EditorIcons::texture, Icons::texture)
    REGISTER_ICON(Thing, _EditorIcons::thing, Icons::thing)
    REGISTER_ICON(Thinker, _EditorIcons::thinker, Icons::thinker)
    REGISTER_ICON(Viewport, _EditorIcons::viewport, Icons::viewport)
    REGISTER_ICON(ViewportTexture, _EditorIcons::viewport_texture, Icons::viewport_texture)
    REGISTER_ICON(Visual2d, _EditorIcons::visual_2d, Icons::visual_2d)
    REGISTER_ICON(Visual3d, _EditorIcons::visual_3d, Icons::visual_3d)

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
