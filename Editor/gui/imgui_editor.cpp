#include "./imgui_editor.hpp"
#include "./imgui_debugger.hpp"
#include "editor_icons.hpp"
#include "assets/icon_uids.hpp"
#include "theatre/editor_theatre.hpp"
#include "assets/icon_uids.hpp"
#include "thirdparty/DearImGui/imgui.h"
#include "thirdparty/DearImGui/imgui_stdlib.h"
#include "thirdparty/DearImGui/imgui_internal.h"
#include <Nostalgia/Nostalgia.hpp>
#include <Nostalgia/application/application.hpp>
#include <Nostalgia/events/event.hpp>
#include <Nostalgia/managers/input_manager.hpp>
#include <Nostalgia/rendering/renderer_api.hpp>
#include <Nostalgia/managers/theatre_manager.hpp>
#include <Nostalgia/settings/engine.hpp>
#include <Nostalgia/settings/world.hpp>
#include <Nostalgia/ui/implementor.hpp>
#include <Nostalgia/things/thing_factory.hpp>
#include <Nostalgia/things/resource_database.hpp>
#include <Nostalgia/things/resources/image_texture.hpp>
#include <Nostalgia/things/thinkers/viewport.hpp>
#include <Nostalgia/things/thinkers/3d/camera_3d.hpp>
#include <Nostalgia/thirdparty/Jolt/Jolt.h>
#include <Nostalgia/thirdparty/Jolt/Core/StringTools.h>

#define REGISTER_ICON(TYPE, VAR_NAME, UID_OUT) \
    m_sEditorIcons[TYPE] = ImageTexture::CreateFromImage(Image::CreateFromFile({VAR_NAME,std::size(VAR_NAME)}));\
    m_sEditorIcons[TYPE]->rename(#TYPE); \
    m_sEditorIconUIDs.emplace(UID_OUT = m_sEditorIcons[TYPE]->uid()); \

using namespace Icons;
using namespace ImGui;

static ImGui_Editor sImGuiEditor{};
ImGui_Editor* g_pImGuiEditor{&sImGuiEditor};

EditorTheatre* ImGui_Editor::m_spEditorTheatre{nullptr};
FileSystem::OverwriteAction ImGui_Editor::m_sCurrentOverwriteAction{FileSystem::RENAME};
std::string ImGui_Editor::m_sScreenshotFilePath{"NostalgiaGoggles_Screenshot.jpg"},
    ImGui_Editor::m_sTheatreFilePath{"theatres/HelloWorld.nt"},
    ImGui_Editor::m_sTheatreFileSavePath{"theatres/SavedTheatre.nt"},
    ImGui_Editor::m_sLastAttemptedTheatreFilePath{m_sTheatreFilePath};
bool ImGui_Editor::m_sTheatreRunning{false},
    ImGui_Editor::m_sInspectingNewThinker{false},
    ImGui_Editor::m_sInspectingNewResource{false},
    ImGui_Editor::m_sAddThing{false};
std::unordered_map<PID, Shared<ImageTexture>> ImGui_Editor::m_sEditorIcons{};
std::unordered_set<ID> ImGui_Editor::m_sEditorIconUIDs{};

static ImGuiChildFlags sResizableChildWithBorder{ImGuiChildFlags_Borders |
    ImGuiChildFlags_ResizeY |
    ImGuiChildFlags_ResizeX};
static float s2DCameraZoomFactor{0.2f}, s2DCameraMovementSpeed{1.0f};
static bool sEditorJustLoaded{false};

static bool s_TreeNodeEx(const char*, ImGuiTreeNodeFlags);

void ImDrawCallback_ImplGL_EnableSRGB(const ImDrawList*, const ImDrawCmd*)
{ RendererAPI::Get()->SetFramebufferSRGB(true); }

void ImDrawCallback_ImplGL_DisableSRGB(const ImDrawList*, const ImDrawCmd*)
{ RendererAPI::Get()->SetFramebufferSRGB(false); }

void ImGui_Editor::Init()
{
    PRINT_PRETTY_FUNCTION;
    REGISTER_ICON(ThingType::Actor2D, _EditorIcons::actor_2d, Icons::actor_2d)
    REGISTER_ICON(ThingType::Actor3D, _EditorIcons::actor_3d, Icons::actor_3d)
    REGISTER_ICON(ThingType::ArrayMesh, _EditorIcons::array_mesh, Icons::array_mesh)
    REGISTER_ICON(ThingType::Camera2D, _EditorIcons::camera_2d, Icons::camera_2d)
    REGISTER_ICON(ThingType::Camera3D, _EditorIcons::camera_3d, Icons::camera_3d)
    REGISTER_ICON(ThingType::Collider3D, _EditorIcons::collider_3d, Icons::collider_3d)
    REGISTER_ICON(ThingType::Cubemap, _EditorIcons::cubemap, Icons::cubemap)
    REGISTER_ICON(ThingType::DirectionalLight3D, _EditorIcons::directional_light_3d, Icons::directional_light_3d)
    REGISTER_ICON(ThingType::Font, _EditorIcons::font, Icons::font)
    REGISTER_ICON(ThingType::Image, _EditorIcons::image, Icons::image)
    REGISTER_ICON(ThingType::ImageTexture, _EditorIcons::image_texture, Icons::image_texture)
    REGISTER_ICON(ThingType::Light3D, _EditorIcons::light_3d, Icons::light_3d)
    REGISTER_ICON(ThingType::Material, _EditorIcons::material, Icons::material)
    REGISTER_ICON(ThingType::Mesh, _EditorIcons::mesh, Icons::mesh)
    REGISTER_ICON(ThingType::MeshInstance3D, _EditorIcons::mesh_instance_3d, Icons::mesh_instance_3d)
    REGISTER_ICON(ThingType::NostalgiaPlayer, _EditorIcons::nostalgia_player_3d, Icons::nostalgia_player_3d)
    REGISTER_ICON(ThingType::PointLight3D, _EditorIcons::point_light_3d, Icons::point_light_3d)
    REGISTER_ICON(ThingType::Resource, _EditorIcons::resource, Icons::resource)
    REGISTER_ICON(ThingType::SpotLight3D, _EditorIcons::spot_light_3d, Icons::spot_light_3d)
    REGISTER_ICON(ThingType::Sprite2D, _EditorIcons::sprite_2d, Icons::sprite_2d)
    REGISTER_ICON(ThingType::Sprite3D, _EditorIcons::sprite_3d, Icons::sprite_3d)
    REGISTER_ICON(ThingType::Text2D, _EditorIcons::text_2d, Icons::text_2d)
    REGISTER_ICON(ThingType::Texture, _EditorIcons::texture, Icons::texture)
    REGISTER_ICON(ThingType::Thing, _EditorIcons::thing, Icons::thing)
    REGISTER_ICON(ThingType::Thinker, _EditorIcons::thinker, Icons::thinker)
    REGISTER_ICON(ThingType::Viewport, _EditorIcons::viewport, Icons::viewport)
    REGISTER_ICON(ThingType::ViewportTexture, _EditorIcons::viewport_texture, Icons::viewport_texture)
    REGISTER_ICON(ThingType::Visual2D, _EditorIcons::visual_2d, Icons::visual_2d)
    REGISTER_ICON(ThingType::Visual3D, _EditorIcons::visual_3d, Icons::visual_3d)
}

void ImGui_Editor::Shutdown()
{
    PRINT_PRETTY_FUNCTION;
    m_sEditorIcons.clear();
    m_sEditorIconUIDs.clear();
}

void ImGui_Editor::TheatreEntered()
{
    ID _viewport_uid_3d{}, _viewport_uid_2d{};
    if(not Settings::Engine::IsEditorHint or not (
            m_spEditorTheatre = dynamic_cast<EditorTheatre*>(Theatre::Current())))
        { return; }

    if(not Theatre::Current()->ThingExists("Editor-3DViewport"))
        { _viewport_uid_3d = Theatre::Current()->CreateThing({ThingType::Viewport, "Editor-3DViewport"}); }
    else
        { _viewport_uid_3d = Theatre::Current()->GetThing("Editor-3DViewport")->uid(); }

    if(not Theatre::Current()->ThingExists("Editor-2DViewport"))
        { _viewport_uid_2d = Theatre::Current()->CreateThing({ThingType::Viewport, "Editor-2DViewport"}); }
    else
        { _viewport_uid_2d = Theatre::Current()->GetThing("Editor-2DViewport")->uid(); }

    if(not Theatre::Current()->ThingExists("EditorCamera3D"))
    {
        TheatreFile::ThingData cam_dat{ThingType::Camera3D, "EditorCamera3D"};
        cam_dat.set_variable(glm::vec3{1.2f, 6.3f, 5.6f}, "Position");
        cam_dat.set_variable(glm::vec3{-26.8f, 7.7f, 0.0f}, "RotationDegrees");
        cam_dat.set_variable(true, "UseDefaultSkybox");
        cam_dat.set_variable(true, "Current");
        Theatre::Current()->CreateThing(cam_dat);
    }

    if(not Theatre::Current()->ThingExists("EditorCamera2D"))
    {
        TheatreFile::ThingData cam_dat{ThingType::Camera2D, "EditorCamera2D"};
        cam_dat.set_variable(true, "Current");
        Theatre::Current()->CreateThing(cam_dat);
    }

    Theatre::Current()->SetParent(Theatre::Current()->GetThing("EditorCamera3D")->uid(), _viewport_uid_3d);
    Theatre::Current()->SetParent(Theatre::Current()->GetThing("EditorCamera2D")->uid(), _viewport_uid_2d);
    m_spEditorTheatre->SetEditorViewports(_viewport_uid_3d, _viewport_uid_2d);
    mEditorTheatreData = m_spEditorTheatre->InitialState();
}

void ImGui_Editor::TheatreExited()
{
    mInspectingResourceUID = {};
    mInspectingThinkerUID = {};
}

void ImGui_Editor::Input(InputEvent* event)
{
    if(event->IsJustPressed(Key::D) and event->IsModifierActive(Key::Mod_Control))
        { ImGui_Debugger::m_sDebugWindowOpened = !ImGui_Debugger::m_sDebugWindowOpened; }
    else if(event->IsJustPressed(Key::F2) and m_sTheatreRunning)
    {
        if(Settings::Engine::IsEditorHint)
            { m_spEditorTheatre->m_pEditor3DViewport->GetImage()->SaveJPG(m_sScreenshotFilePath); }
        else
            { Theatre::Current()->TakeScreenshot()->SaveJPG(m_sScreenshotFilePath); }
    }
    else if(event->IsJustPressed(Key::F3))
    {
        if(not Settings::Engine::IsEditorHint)
            { return; }
        if(not m_sTheatreRunning)
        {
            LoadTheatre(true);
            return;
        }
        mEditorTheatreData = m_spEditorTheatre->CurrentState();
        LoadTheatre(false);
    }
    else if(event->IsJustPressed(Key::F4))
    {
        Manager::ShutdownTheatre();
        Settings::Engine::IsEditorHint = true;
        MainWindow()->SetMouseMode(IWindow::MOUSE_MODE_VISIBLE);
    }
    else if(event->IsJustPressed(Key::F5))
        { LoadEditorTheatre(m_sTheatreRunning and not Settings::Engine::IsEditorHint); }
    else if(event->IsJustPressed(Key::F6))
        { Theatre::Current()->SaveToFile(m_sTheatreFileSavePath, m_sCurrentOverwriteAction); }
    else if(event->IsJustPressed(Key::A)
        and event->IsModifierActive(Key::Mod_Control)
        and event->IsModifierActive(Key::Mod_Shift)
        and m_sTheatreRunning and Settings::Engine::IsEditorHint)
            { m_sAddThing = true; }
}

void ImGui_Editor::Update()
{
    m_sTheatreRunning = Manager::GetTheatreState() == ManagerEnums::IN_LEVEL;
    if(not Settings::Engine::IsEditorHint and MainWindow()->GetMouseMode() != IWindow::MOUSE_MODE_VISIBLE)
        { return; }
    SetNextWindowPos({0,0}, ImGuiCond_Once);
    SetNextWindowSize({static_cast<float>(MainWindow()->GetWidth()),
        static_cast<float>(MainWindow()->GetHeight())});
    if(not Begin("Nostalgia Editor",
            nullptr,
            ImGuiWindowFlags_MenuBar |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoBringToFrontOnFocus))
        { End(); }
    ThingAdder();
    BringWindowToDisplayBack(GetCurrentWindow());
    if(BeginMenuBar())
    {
        if(BeginMenu("File"))
        {
            SeparatorText("Theatre Quick-Actions");
            if(MenuItem("Edit Theatre", "F5"))
                { LoadEditorTheatre(Theatre::Current() and Theatre::Current()->IsStarted()); }
            if(MenuItem("Run Edited Theatre", "F3"))
                {
                    if(not m_sTheatreRunning or not Settings::Engine::IsEditorHint)
                        { return; }
                    mEditorTheatreData = m_spEditorTheatre->CurrentState();
                    LoadTheatre(false);
                }
            BeginDisabled(not m_sTheatreRunning);
                if(MenuItem("Save Theatre to File", "F6"))
                    { Theatre::Current()->SaveToFile(m_sTheatreFileSavePath, m_sCurrentOverwriteAction); }
                if(MenuItem("Close Theatre", "F4"))
                {
                    Manager::ShutdownTheatre();
                    Settings::Engine::IsEditorHint = true;
                }
            EndDisabled();
            Separator();
            if(MenuItem("Quit", "CTRL+Q"))
                { Application()->Stop(); }
            ImGui::EndMenu();
        }
        if(BeginMenu("Edit"))
        {
            BeginDisabled(not m_sTheatreRunning or not Settings::Engine::IsEditorHint);
                if(MenuItem("Add Thing", "CTRL+SHIFT+A"))
                    { m_sAddThing = true; }
            EndDisabled();
            ImGui::EndMenu();
        }
        if(BeginMenu("Window"))
        {
            if(MenuItem("Debug Menu", "CTRL+D"))
                { ImGui_Debugger::m_sDebugWindowOpened = true; }
            if(MenuItem("Debug Console", "~"))
                { ImGui_Debugger::m_sDebugConsoleOpened = true; }
            ImGui::EndMenu();
        }
        if(BeginMenu("Preferences"))
        {
            SliderFloat("2D Camera Movement Speed", &s2DCameraMovementSpeed, 1.0f, 100.0f);
            SliderFloat("2D Camera Zooming Factor", &s2DCameraZoomFactor, 0.001f, 0.999f);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    if(BeginTabBar("EditorTabs"))
    {
        auto _editor_flags{ImGuiTabItemFlags_None};
        if(sEditorJustLoaded)
        {
            _editor_flags = ImGuiTabItemFlags_SetSelected;
            sEditorJustLoaded = false;
        }
        if(m_sTheatreRunning and Settings::Engine::IsEditorHint
            and BeginTabItem("Editor", nullptr, _editor_flags))
        {
            TheatreViewport();
            SameLine();
            TheatreInspector();
            EndTabItem();
        }
        if(BeginTabItem("Theatre Loading"))
        {
            TheatreLoadingWindow();
            EndTabItem();
        }
        EndTabBar();
    }
    End();
}

void ImGui_Editor::LoadEditorTheatre(bool inContinue)
{
    sEditorJustLoaded = Settings::Engine::IsEditorHint;
    g_pTheatreManager->SetNextTheatreType<EditorTheatre>();
    if(inContinue)
        { g_pTheatreManager->LoadFromData(mEditorTheatreData); }
    else
        { g_pTheatreManager->LoadFromFile(m_sTheatreFilePath); }
    MainWindow()->SetMouseMode(IWindow::MOUSE_MODE_VISIBLE);
}

void ImGui_Editor::LoadTheatre(bool inLoadFile)
{
    Settings::Engine::IsEditorHint = false;
    g_pTheatreManager->SetNextTheatreType<Theatre>();
    if(inLoadFile)
        { g_pTheatreManager->LoadFromFile(m_sTheatreFilePath); }
    else
        { g_pTheatreManager->LoadFromData(mEditorTheatreData); }
}

void ImGui_Editor::TheatreViewport()
{
    BeginChild("LeftSide", {784, 965}, sResizableChildWithBorder);
        if(BeginTabBar("Viewports"))
        {
            if(BeginTabItem("3D"))
            {
                Viewport3DWindow();
                EndTabItem();
            }
            if(BeginTabItem("2D"))
            {
                Viewport2DWindow();
                EndTabItem();
            }
            EndTabBar();
        }
    EndChild();
}

void ImGui_Editor::TheatreInspector()
{
    if(not Theatre::Current()->IsStarted())
        { mInspectingResourceUID = mInspectingThinkerUID = ID::Invalid; }
    else
    {
        BeginChild("TheatreInspector");
            if(CollapsingHeader("Resources"))
            {
                for(FAUTO uid : Theatre::Current()->ResourceUIDs())
                {
                    if(_select_thing(uid))
                    {
                        mInspectingResourceUID   = uid;
                        m_sInspectingNewResource = true;
                    }
                }
            }
            SetNextItemOpen(true, ImGuiCond_Once);
            if(CollapsingHeader("Thinkers"))
            {
                for(FAUTO uid : Theatre::Current()->ThinkerUIDs())
                {
                    if(not Theatre::Current()->GetParent(uid).invalid())
                        { continue; }
                    _thinker_tree_branch(uid);
                }
            }
            if(not mInspectingThinkerUID.invalid())
                { InspectThinker(); }
            if(not mInspectingResourceUID.invalid())
                { InspectResource(); }
        EndChild();
    }
}

bool ImGui_Editor::_select_thing(ID inUID)
{
    auto _thing{Theatre::Current()->GetThing(inUID)};
    ImGui::Image((ImTextureRef)GetIconTextureBufferID(Theatre::Current()->TypeOf(inUID)),
        {30, 30},
        {0, 1},
        {1, 0});
    SameLine();
    PushID(inUID());
    if(Button(_thing->c_name(), {0, 30}))
    {
        PopID();
        return true;
    }
    PopID();
    if(auto _thinker{DCast<Thinker>(_thing)})
        { _thinker->IsHighlighted(IsItemHovered()); }
    return false;
}

void ImGui_Editor::_thinker_tree_branch(ID inUID)
{
    auto children{Theatre::Current()->GetChildren(inUID)};
    if(s_TreeNodeEx(Theatre::Current()->GetThing(inUID)->c_name(),
        (children.empty()) ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_None))
    {
        SameLine();
        if(_select_thing(inUID))
        {
            mInspectingThinkerUID   = inUID;
            m_sInspectingNewThinker = true;
        }
        for(ID child : children)
            { _thinker_tree_branch(child); }
        TreePop();
    }
    else
    {
        SameLine();
        if(_select_thing(inUID))
        {
            mInspectingThinkerUID   = inUID;
            m_sInspectingNewThinker = true;
        }
    }
}

void ImGui_Editor::Viewport3DWindow()
{
    static ImVec2 _viewport_window_size{}, _viewport_size{};
    BeginChild("Editor-3D-Viewport",
        {768, 519},
        sResizableChildWithBorder,
        ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse);
        _viewport_size = {(float)m_spEditorTheatre->m_pEditor3DViewport->Size().w(),
                (float)m_spEditorTheatre->m_pEditor3DViewport->Size().h()};
        GetWindowDrawList()->AddCallback(ImDrawCallback_ImplGL_EnableSRGB, nullptr);
        ImGui::Image((ImTextureRef)m_spEditorTheatre->m_pEditor3DViewport->GetTextureBufferID(),
            _viewport_size,
            {0, 1},
            {1, 0});
        GetWindowDrawList()->AddCallback(ImDrawCallback_ImplGL_DisableSRGB, nullptr);
        _viewport_window_size = GetWindowSize();
        if(_viewport_size != _viewport_window_size)
        {
            m_spEditorTheatre->m_pEditor3DViewport
                ->SetSize({_viewport_window_size[0], _viewport_window_size[1]});
        }
    EndChild();
    static bool camera_moving{false};
    if((IsItemHovered() or camera_moving))
    {
        if(InputManager::IsKeyDown(Key::MouseLeft) and !camera_moving)
        {
            UI_Implementor::SetGlobalCanHandleEvents(false);
            camera_moving = true;
            MainWindow()->SetMouseMode(IWindow::MouseMode::MOUSE_MODE_DISABLED);
        }
        if(InputManager::IsKeyDown(Key::MouseLeft))
        {
            auto motion{InputManager::MouseMotion()};
            m_spEditorTheatre->m_pEditorCamera3D
                ->SetRotationDegrees(m_spEditorTheatre->m_pEditorCamera3D->RotationDegrees() -
                    (glm::vec3{motion.y(), motion.x(), 0.0f} * 0.1f));
            glm::vec2 movement_direction{
                InputManager::IsActionDown("+right") - InputManager::IsActionDown("+left"),
                InputManager::IsActionDown("+forward") - InputManager::IsActionDown("+backward")};

            m_spEditorTheatre->m_pEditorCamera3D->SetPosition(m_spEditorTheatre->m_pEditorCamera3D->Position() +
                ((
                    ((m_spEditorTheatre->m_pEditorCamera3D->Quaternion() * Settings::World::Front()) *
                        movement_direction[1]) +
                    ((m_spEditorTheatre->m_pEditorCamera3D->Quaternion() * Settings::World::Right()) *
                        movement_direction[0])
                ) * 0.1f)
            );
        }
    }
    if(InputManager::IsKeyUp(Key::MouseLeft) and camera_moving)
    {
        UI_Implementor::SetGlobalCanHandleEvents(true);
        camera_moving = false;
        MainWindow()->SetMouseMode(IWindow::MouseMode::MOUSE_MODE_VISIBLE);
    }
}

void ImGui_Editor::Viewport2DWindow()
{
    static ImVec2 _viewport_window_size{}, _viewport_size{};
    BeginChild("Editor-2D-Viewport",
        {768, 519},
        sResizableChildWithBorder,
        ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse);
        _viewport_size = {(float)m_spEditorTheatre->m_pEditor2DViewport->Size().w(),
                (float)m_spEditorTheatre->m_pEditor2DViewport->Size().h()};
        GetWindowDrawList()->AddCallback(ImDrawCallback_ImplGL_EnableSRGB, nullptr);
        ImGui::Image((ImTextureRef)m_spEditorTheatre->m_pEditor2DViewport->GetTextureBufferID(),
            _viewport_size,
            {0, 1},
            {1, 0});
        GetWindowDrawList()->AddCallback(ImDrawCallback_ImplGL_DisableSRGB, nullptr);
        _viewport_window_size = GetWindowSize();
        if(_viewport_size != _viewport_window_size)
        {
            m_spEditorTheatre->m_pEditor2DViewport
                ->SetSize({_viewport_window_size[0], _viewport_window_size[1]});
        }
    EndChild();
    static bool camera_moving{false};
    if((IsItemHovered() or camera_moving))
    {
        if(InputManager::IsKeyDown(Key::MouseLeft) and not camera_moving)
        {
            UI_Implementor::SetGlobalCanHandleEvents(false);
            camera_moving = true;
            MainWindow()->SetMouseMode(IWindow::MouseMode::MOUSE_MODE_DISABLED);
        }
        if(InputManager::IsKeyDown(Key::MouseLeft))
        {
            auto _scroll{InputManager::ScrollOffset()[1]};
            auto _zoom{m_spEditorTheatre->m_pEditorCamera2D->Zoom()};
            float _zoom_min{(_zoom[1] > _zoom[0]) ? _zoom[0] : _zoom[1]};
            if(_scroll)
            {
                float _zoom_factor{static_cast<float>(_scroll) * s2DCameraZoomFactor * abs(_zoom_min)};
                if(_scroll < 0 and _zoom_min < 1)
                    { _zoom_factor *= 0.5f; }
                m_spEditorTheatre->m_pEditorCamera2D->SetZoom(_zoom += glm::vec2{_zoom_factor});
            }
            if(auto motion{InputManager::MouseMotion()}; not motion.is_zero())
            {
                m_spEditorTheatre->m_pEditorCamera2D
                    ->SetPosition(m_spEditorTheatre->m_pEditorCamera2D->Position() -
                        glm::vec2{motion.x(), -motion.y()} * s2DCameraMovementSpeed / _zoom);
            }
        }
    }
    if(InputManager::IsKeyUp(Key::MouseLeft) and camera_moving)
    {
        UI_Implementor::SetGlobalCanHandleEvents(true);
        camera_moving = false;
        MainWindow()->SetMouseMode(IWindow::MouseMode::MOUSE_MODE_VISIBLE);
    }
}

// https://github.com/ocornut/imgui/issues/282#issuecomment-1964859909
bool s_TreeNodeEx(const char* inLabel, ImGuiTreeNodeFlags inFlags)
{
    Farg<ImGuiContext> g{*ImGui::GetCurrentContext()};
    ImGuiWindow* _window{g.CurrentWindow};

    ImVec2 pos{_window->DC.CursorPos};
    ImGuiID _id{_window->GetID(inLabel)};
    ImVec2 _button_min{pos[0] + g.FontSize, pos[1] + g.Style.FramePadding[1] * 2 + g.Style.CellPadding[1]};
    ImRect _bb{_button_min, _button_min + ImVec2{g.FontSize, g.FontSize}};
    bool _opened{TreeNodeGetOpen(_id)},
        _hovered{},
        _held{};

    if(not (inFlags & ImGuiTreeNodeFlags_Leaf))
    {
        if(ButtonBehavior(_bb,
                _id,
                &_hovered,
                &_held,
                ImGuiButtonFlags_PressedOnClick))
            { _window->DC.StateStorage->SetInt(_id, not _opened); }

        RenderArrow(_window->DrawList,
            _button_min,
            GetColorU32(ImGuiCol_Text),
            (_opened) ? ImGuiDir_Down : ImGuiDir_Right,
            1.0f);
    }

    ItemSize(ImRect{_bb.Min, _bb.Max + g.Style.FramePadding * 2}, g.Style.FramePadding.y);
    ItemAdd(_bb, _id);

    if(_opened)
        { TreePush(inLabel); }

    return _opened;
}

uint ImGui_Editor::GetIconTextureBufferID(FPID inType)
{
    if(not m_sEditorIcons.contains(inType))
        { return 0; }
    return m_sEditorIcons.at(inType)->Buffer()->GetID();
}

void ImGui_Editor::SelectThing(const char* inLabel, ID& ioUID, bool& outChanged)
{
    static const int _max_per_row{3};
    static std::string _name_input{};
    auto _label{std::format("Thing Selection##{}", ioUID())};
    if(Button(inLabel))
        { OpenPopup(_label.data()); }
    if(BeginPopup(_label.data()))
    {
        InputText("Search", &_name_input);

        auto _theatre_uids{Theatre::Current()->ThingUIDs()};
        size_t _back_of_theatre{_theatre_uids.size()};
        auto _resource_uids{ResourceDatabase::GetUIDs()};
        _theatre_uids.insert(_theatre_uids.cend(), _resource_uids.cbegin(), _resource_uids.cend());
        int _counter{0};
        for(size_t i{0}; i < _theatre_uids.size(); ++i)
        {
            ID uid{_theatre_uids.at(i)};
            if(uid == _theatre_uids.front())
                { SeparatorText("Theatre"); }
            else if(i == _back_of_theatre)
                { SeparatorText("ResourceDatabase"); }
            std::string _name{Theatre::Current()->GetName(uid)};
            if(_name.empty() or
                (not _name_input.empty() and not GetLowercase(_name).contains(GetLowercase(_name_input))))
                { continue; }
            if(Button(_name.data(), {(700.0f / _max_per_row) - 5.0f, 0.0f}))
            {
                EndPopup();
                ioUID = uid;
                outChanged = true;
                _name_input.clear();
                return;
            }
            Theatre::Current()->GetThinker(uid)->IsHighlighted(IsItemHovered());
            if(++_counter < _max_per_row and i+1 != _back_of_theatre) { SameLine(); }
            else { _counter = 0; }
        }
        EndPopup();
    }
}
