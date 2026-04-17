#include "./imgui_editor.hpp"
#include "./imgui_debugger.hpp"
#include "theatre/editor_theatre.hpp"
#include "assets/icon_uids.hpp"
#include "thirdparty/DearImGui/imgui.h"
#include "thirdparty/DearImGui/imgui_stdlib.h"
#include "thirdparty/DearImGui/imgui_internal.h"
#include <Nostalgia/Nostalgia.hpp>
#include <Nostalgia/application/application.hpp>
#include <Nostalgia/events/event.hpp>
#include <Nostalgia/managers/input_manager.hpp>
#include <Nostalgia/managers/render_manager.hpp>
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

using namespace Icons;
using namespace ImGui;

static ImGui_Editor sImGuiEditor{};
ImGui_Editor* g_pImGuiEditor{&sImGuiEditor};

EditorTheatre* ImGui_Editor::m_spEditorTheatre{nullptr};
Theatre::FileOverwriteAction ImGui_Editor::m_sCurrentFileOverwriteAction{Theatre::RENAME};
std::string ImGui_Editor::m_sTheatreFilePath{"theatres/HelloWorld.nt"},
    ImGui_Editor::m_sTheatreFileSavePath{"theatres/SavedTheatre.nt"},
    ImGui_Editor::m_sLastAttemptedTheatreFilePath{m_sTheatreFilePath};
bool ImGui_Editor::m_sTheatreRunning{false},
    ImGui_Editor::m_sInspectingNewThinker{false},
    ImGui_Editor::m_sInspectingNewResource{false},
    ImGui_Editor::m_sAddThing{false};

static ImGuiChildFlags sResizableChildWithBorder{ImGuiChildFlags_Borders |
    ImGuiChildFlags_ResizeY |
    ImGuiChildFlags_ResizeX};
static float s2DCameraZoomFactor{0.2f}, s2DCameraMovementSpeed{1.0f};
static bool sEditorJustLoaded{false};

static bool s_TreeNodeEx(const char*, ImGuiTreeNodeFlags);

void ImDrawCallback_ImplGL_EnableSRGB(const ImDrawList*, const ImDrawCmd*)
{ g_pRenderManager->GetAPI()->SetFramebufferSRGB(true); }

void ImDrawCallback_ImplGL_DisableSRGB(const ImDrawList*, const ImDrawCmd*)
{ g_pRenderManager->GetAPI()->SetFramebufferSRGB(false); }

void ImGui_Editor::Init()
{ PRINT_PRETTY_FUNCTION; }

void ImGui_Editor::Shutdown()
{ PRINT_PRETTY_FUNCTION; }

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
{}

void ImGui_Editor::Input(InputEvent* event)
{
    if(event->IsJustPressed(Key::D) and event->IsModifierActive(Key::Mod_Control))
        { ImGui_Debugger::m_sDebugWindowOpened = !ImGui_Debugger::m_sDebugWindowOpened; }
    else if(event->IsJustPressed(Key::F3))
    {
        bool _started{Theatre::Current()->IsStarted()};
        if(not _started or not Settings::Engine::IsEditorHint)
            { return; }
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
        { Theatre::Current()->SaveToFile(m_sTheatreFileSavePath, m_sCurrentFileOverwriteAction); }
    else if(event->IsJustPressed(Key::A)
        and event->IsModifierActive(Key::Mod_Control)
        and event->IsModifierActive(Key::Mod_Shift)
        and m_sTheatreRunning and Settings::Engine::IsEditorHint)
        { m_sAddThing = true; }
}

void ImGui_Editor::Update()
{
    m_sTheatreRunning = Manager::GetTheatreState() == ManagerEnums::IN_LEVEL;
    if(not Settings::Engine::IsEditorHint)
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
                    { Theatre::Current()->SaveToFile(m_sTheatreFileSavePath, m_sCurrentFileOverwriteAction); }
                if(MenuItem("Close Theatre", "F4"))
                {
                    Manager::ShutdownTheatre();
                    Settings::Engine::IsEditorHint = true;
                }
            EndDisabled();
            Separator();
            if(MenuItem("Quit", "CTRL+Q"))
                { Application()->Stop(); }
            EndMenu();
        }
        if(BeginMenu("Edit"))
        {
            BeginDisabled(not m_sTheatreRunning or not Settings::Engine::IsEditorHint);
                if(MenuItem("Add Thing", "CTRL+SHIFT+A"))
                    { m_sAddThing = true; }
            EndDisabled();
            EndMenu();
        }
        if(BeginMenu("Window"))
        {
            if(MenuItem("Debug Menu", "CTRL+D"))
                { ImGui_Debugger::m_sDebugWindowOpened = true; }
            if(MenuItem("Debug Console", "~"))
                { ImGui_Debugger::m_sDebugConsoleOpened = true; }
            EndMenu();
        }
        if(BeginMenu("Preferences"))
        {
            SliderFloat("2D Camera Movement Speed", &s2DCameraMovementSpeed, 1.0f, 100.0f);
            SliderFloat("2D Camera Zooming Factor", &s2DCameraZoomFactor, 0.001f, 0.999f);
            EndMenu();
        }
        EndMenuBar();
    }
    if(BeginTabBar("EditorTabs"))
    {
        auto _editor_flags{ImGuiTabItemFlags_None};
        if(sEditorJustLoaded)
        {
            _editor_flags = ImGuiTabItemFlags_SetSelected;
            sEditorJustLoaded = false;
        }
        if(m_sTheatreRunning and BeginTabItem("Editor", nullptr, _editor_flags))
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
    sEditorJustLoaded = Settings::Engine::IsEditorHint = true;
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
                    { _select_thing(uid, mInspectingResourceUID); }
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

void ImGui_Editor::_select_thing(ID inUID, ID& outUID)
{
    auto _thing{Theatre::Current()->GetThing(inUID)};
    ImGui::Image((ImTextureRef)GetIconTextureBufferID(Theatre::Current()->TypeOf(inUID)),
        {30, 30},
        {0, 1},
        {1, 0});
    SameLine();
    PushID(inUID());
    if(Button(_thing->c_name(), {0, 30}))
        { outUID = inUID; }
    PopID();
    if(auto _thinker{DCast<Thinker>(_thing)})
        { _thinker->IsHighlighted(IsItemHovered()); }
}

void ImGui_Editor::_thinker_tree_branch(ID inUID)
{
    auto children{Theatre::Current()->GetChildren(inUID)};
    if(s_TreeNodeEx(Theatre::Current()->GetThing(inUID)->c_name(),
        (children.empty()) ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_None))
    {
        SameLine();
        _select_thing(inUID, mInspectingThinkerUID);
        for(ID child : children)
            { _thinker_tree_branch(child); }
        TreePop();
    }
    else
    {
        SameLine();
        _select_thing(inUID, mInspectingThinkerUID);
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
        ImGui::Image((ImTextureID)m_spEditorTheatre->m_pEditor3DViewport->Framebuffer()->TextureID(),
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
        ImGui::Image((ImTextureID)m_spEditorTheatre->m_pEditor2DViewport->Framebuffer()->TextureID(),
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
    if(not g_pRenderManager->GetAPI() or not g_pRenderManager->GetAPI()->IsRunning())
        { return 0; }
    auto _type{ThingFactory::GetClosestType(inType)};
    ID _uid{Icons::thing};
    if(_type == ThingType::Thing)
        _uid = Icons::thing;
    else if(_type == ThingType::Resource)
        _uid = Icons::resource;
    else if(_type == ThingType::Font)
        _uid = Icons::font;
    else if(_type == ThingType::Mesh)
        _uid = Icons::mesh;
    else if(_type == ThingType::ArrayMesh)
        _uid = Icons::array_mesh;
    else if(_type == ThingType::Texture)
        _uid = Icons::texture;
    else if(_type == ThingType::Cubemap)
        _uid = Icons::cubemap;
    else if(_type == ThingType::ViewportTexture)
        _uid = Icons::viewport_texture;
    else if(_type == ThingType::ImageTexture)
        _uid = Icons::image_texture;
    else if(_type == ThingType::Material)
        _uid = Icons::material;
    else if(_type == ThingType::Image)
        _uid = Icons::image;
    else if(_type == ThingType::Thinker)
        _uid = Icons::thinker;
    else if(_type == ThingType::Viewport)
        _uid = Icons::viewport;
    else if(_type == ThingType::Actor3D)
        _uid = Icons::actor_3d;
    else if(_type == ThingType::NostalgiaPlayer)
        _uid = Icons::nostalgia_player_3d;
    else if(_type == ThingType::Camera3D)
        _uid = Icons::camera_3d;
    else if(_type == ThingType::Collider3D)
        _uid = Icons::collider_3d;
    else if(_type == ThingType::Visual3D)
        _uid = Icons::visual_3d;
    else if(_type == ThingType::MeshInstance3D)
        _uid = Icons::mesh_instance_3d;
    else if(_type == ThingType::Sprite3D)
        _uid = Icons::sprite_3d;
    else if(_type == ThingType::Light3D)
        _uid = Icons::light_3d;
    else if(_type == ThingType::PointLight3D)
        _uid = Icons::point_light_3d;
    else if(_type == ThingType::SpotLight3D)
        _uid = Icons::spot_light_3d;
    else if(_type == ThingType::DirectionalLight3D)
        _uid = Icons::directional_light_3d;
    else if(_type == ThingType::Actor2D)
        _uid = Icons::actor_2d;
    else if(_type == ThingType::Camera2D)
        _uid = Icons::camera_2d;
    else if(_type == ThingType::Visual2D)
        _uid = Icons::visual_2d;
    else if(_type == ThingType::Sprite2D)
        _uid = Icons::sprite_2d;
    else if(_type == ThingType::Text2D)
        _uid = Icons::text_2d;
    return ResourceDatabase::GetResource<ImageTexture>(_uid)->GetBuffer()->ID();
}

void ImGui_Editor::SelectThing(const char* inLabel, ID& ioUID, bool& outChanged)
{
    static const int _max_per_row{3};
    static std::string _name_input{};
    PushID(ioUID());
        if(Button(inLabel))
            { OpenPopup("Thing Selection"); }
        if(BeginPopup("Thing Selection"))
        {
            InputText("Search", &_name_input);

            auto uids{Theatre::Current()->ThingUIDs()};
            int _counter{0};
            for(ID uid : uids)
            {
                std::string _name{Theatre::Current()->GetName(uid)};
                if(_name.empty() or
                    (not _name_input.empty() and not JPH::ToLower(_name).contains(JPH::ToLower(_name_input))))
                    { continue; }
                if(Button(_name.data(), {(700.0f / _max_per_row) - 5.0f, 0.0f}))
                {
                    EndPopup();
                    PopID();
                    ioUID = uid;
                    outChanged = true;
                    _name_input.clear();
                    return;
                }
                Theatre::Current()->GetThinker(uid)->IsHighlighted(IsItemHovered());
                if(++_counter < _max_per_row) { SameLine(); }
                else { _counter = 0; }
            }
            EndPopup();
        }
    PopID();
}
