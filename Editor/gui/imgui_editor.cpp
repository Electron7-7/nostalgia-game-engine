#include "./imgui_editor.hpp"
#include "./imgui_debugger.hpp"
#include "editor_icons.hpp"
#include "new_editor_icons.hpp"
#include "thirdparty/ImGuiFileDialog/ImGuiFileDialog.h"
#include "theatre/editor_theatre.hpp"
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
#include <Nostalgia/things/resources/image_texture.hpp>
#include <Nostalgia/things/thinkers/viewport.hpp>
#include <Nostalgia/things/thinkers/3d/ramiel.hpp>
#include <Nostalgia/things/thinkers/3d/camera_3d.hpp>
#include <Nostalgia/thirdparty/Jolt/Jolt.h>
#include <Nostalgia/thirdparty/Jolt/Core/StringTools.h>

#define REGISTER_ICON(TYPE, VAR_NAME) \
    mEditorIcons[TYPE] = ImageTexture::CreateFromImage(\
        Image::CreateFromFile({_EditorIcons::VAR_NAME,std::size(_EditorIcons::VAR_NAME)})); \
    mEditorIcons[TYPE]->rename(#TYPE); \

#define REGISTER_NEW_ICON(TYPE, VAR_NAME) \
    mNewEditorIcons[TYPE] = ImageTexture::CreateFromImage(\
        Image::CreateFromFile({_NewEditorIcons::VAR_NAME,std::size(_NewEditorIcons::VAR_NAME)})); \
    mNewEditorIcons[TYPE]->rename(#TYPE);

using namespace ImGui;

static ImGui_Editor sImGuiEditor{};
ImGui_Editor* g_pImGuiEditor{&sImGuiEditor};

EditorTheatre* ImGui_Editor::m_spEditorTheatre{nullptr};
FileSystem::OverwriteAction ImGui_Editor::m_sCurrentOverwriteAction{FileSystem::RENAME};
std::string ImGui_Editor::m_sScreenshotFilePath{"NostalgiaGoggles_Screenshot.jpg"},
    ImGui_Editor::m_sTheatreFilePath{""};
bool ImGui_Editor::m_sTheatreRunning{false},
    ImGui_Editor::m_sInspectingNewThing{false},
    ImGui_Editor::m_sAddThing{false},
    ImGui_Editor::m_sThingAdderOpened{false},
    ImGui_Editor::m_sCurrentEditorTheatreSaved{false},
    ImGui_Editor::m_sWantNewTheatre{false},
    ImGui_Editor::m_sSavingTheatre{false},
    ImGui_Editor::m_sOpenSaveBeforeExitingPopup{false},
    ImGui_Editor::m_sOpenSavePopup{false},
    sUseNewIcons{false};

static ImGuiChildFlags sResizableChildWithBorder{ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX};
static float s2DCameraZoomFactor{0.2f}, s2DCameraMovementSpeed{1.0f};

static bool s_TreeNodeEx(const char*, ImGuiTreeNodeFlags);

void ImDrawCallback_ImplGL_EnableSRGB(const ImDrawList*, const ImDrawCmd*)
{ RendererAPI::Get()->SetFramebufferSRGB(true); }

void ImDrawCallback_ImplGL_DisableSRGB(const ImDrawList*, const ImDrawCmd*)
{ RendererAPI::Get()->SetFramebufferSRGB(false); }

void ImGui_Editor::Init()
{
    PRINT_PRETTY_FUNCTION;
    REGISTER_ICON(ThingType::Actor2D, actor_2d)
        REGISTER_NEW_ICON(ThingType::Actor2D, actor_2d)
    REGISTER_ICON(ThingType::Actor3D, actor_3d)
        REGISTER_NEW_ICON(ThingType::Actor3D, actor_3d)
    REGISTER_ICON(ThingType::ArrayMesh, array_mesh)
    REGISTER_ICON(ThingType::Camera2D, camera_2d)
    REGISTER_ICON(ThingType::Camera3D, camera_3d)
    REGISTER_ICON(ThingType::Collider3D, collider_3d)
    REGISTER_ICON(ThingType::Cubemap, cubemap)
    REGISTER_ICON(ThingType::DirectionalLight3D, directional_light_3d)
    REGISTER_ICON(ThingType::Font, font)
    REGISTER_ICON(ThingType::Image, image)
    REGISTER_ICON(ThingType::ImageTexture, image_texture)
    REGISTER_ICON(ThingType::Light3D, light_3d)
    REGISTER_ICON(ThingType::Material, material)
    REGISTER_ICON(ThingType::Mesh, mesh)
    REGISTER_ICON(ThingType::MeshInstance3D, mesh_instance_3d)
        REGISTER_NEW_ICON(ThingType::MeshInstance3D, mesh_instance_3d)
    REGISTER_ICON(ThingType::NostalgiaPlayer, nostalgia_player_3d)
    REGISTER_ICON(ThingType::PointLight3D, point_light_3d)
    REGISTER_ICON(ThingType::Resource, resource)
        REGISTER_NEW_ICON(ThingType::Resource, resource)
    REGISTER_ICON(ThingType::SpotLight3D, spot_light_3d)
    REGISTER_ICON(ThingType::Sprite2D, sprite_2d)
        REGISTER_NEW_ICON(ThingType::Sprite2D, sprite_2d)
    REGISTER_ICON(ThingType::Sprite3D, sprite_3d)
    REGISTER_ICON(ThingType::Text2D, text_2d)
    REGISTER_ICON(ThingType::Texture, texture)
    REGISTER_ICON(ThingType::Thing, thing)
        REGISTER_NEW_ICON(ThingType::Thing, thing)
    REGISTER_ICON(ThingType::Thinker, thinker)
        REGISTER_NEW_ICON(ThingType::Thinker, thinker)
    REGISTER_ICON(ThingType::Viewport, viewport)
    REGISTER_ICON(ThingType::ViewportTexture, viewport_texture)
    REGISTER_ICON(ThingType::Visual2D, visual_2d)
        REGISTER_NEW_ICON(ThingType::Visual2D, visual_2d)
    REGISTER_ICON(ThingType::Visual3D, visual_3d)
        REGISTER_NEW_ICON(ThingType::Visual3D, visual_3d)
    REGISTER_ICON(Ramiel::sClassType(), ramiel)
        REGISTER_NEW_ICON(Ramiel::sClassType(), ramiel)

    LoadEditorTheatre(true);
}

void ImGui_Editor::Shutdown()
{
    PRINT_PRETTY_FUNCTION;
    mEditorIcons.clear();
    mNewEditorIcons.clear();
}

void ImGui_Editor::TheatreEntered()
{
    if(not Settings::Engine::IsEditorHint
        or not (m_spEditorTheatre = dynamic_cast<EditorTheatre*>(Theatre::Current())))
    {
        if(mEditorTheatreData.data.empty())
            { mEditorTheatreData = Theatre::Current()->InitialState(); }
        m_spEditorTheatre = nullptr;
        return;
    }
    mEditorTheatreData = m_spEditorTheatre->InitialState();
}

void ImGui_Editor::TheatreExited()
{ mInspectingThingUID = {}; }

void ImGui_Editor::Input(InputEvent* event)
{
    if(event->IsJustPressed(Key::D) and event->IsModifierActive(Key::Mod_Control))
        { ImGui_Debugger::m_sDebugWindowOpened = !ImGui_Debugger::m_sDebugWindowOpened; }
    else if(event->IsJustPressed(Key::N) and event->IsModifierActive(Key::Mod_Control))
    {
        CloseEditorTheatre();
        m_sWantNewTheatre = true;
    }
    else if(event->IsJustPressed(Key::F2) and m_sTheatreRunning and not Settings::Engine::IsEditorHint)
        { Theatre::Current()->TakeScreenshot()->SaveJPG(m_sScreenshotFilePath); }
    else if(event->IsJustPressed(Key::F5))
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
    else if(event->IsJustPressed(Key::F8))
        { LoadEditorTheatre(true); }
    else if(event->IsJustPressed(Key::O) and event->IsModifierActive(Key::Mod_Control))
    {
        if(m_sTheatreRunning and not Settings::Engine::IsEditorHint)
            { LoadEditorTheatre(true); }
        else
        {
            IGFD::FileDialogConfig _config{};
            _config.path = ".";
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey",
                "Choose Theatre File",
                ".nt,.theatre,.nostalgiatheatre",
                _config);
        }
    }
    else if(event->IsJustPressed(Key::S) and event->IsModifierActive(Key::Mod_Control))
        { Theatre::Current()->SaveToFile(m_sTheatreFilePath, m_sCurrentOverwriteAction); }
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
    BringWindowToDisplayBack(GetCurrentWindow());
    ThingAdder();
    if(m_sOpenSaveBeforeExitingPopup)
    {
        OpenPopup("Are You Sure?");
        m_sOpenSaveBeforeExitingPopup = false;
    }
    if(m_sOpenSavePopup)
    {
        OpenPopup("Save Theatre");
        m_sOpenSavePopup = false;
    }
    if(m_sWantNewTheatre)
    {
        AreYouSurePopup();
        SaveTheatrePopup();
        if(not m_sSavingTheatre and m_sWantNewTheatre)
        {
            CloseEditorTheatre();
            LoadEditorTheatre(true);
            m_sWantNewTheatre = false;
        }
    }
    if(BeginMenuBar())
    {
        if(BeginMenu("File"))
        {
            if(MenuItem("New Theatre", "CTRL+N"))
            {
                CloseEditorTheatre();
                m_sWantNewTheatre = true;
            }
            if(MenuItem("Open Theatre File", "CTRL+O"))
            {
                IGFD::FileDialogConfig _config{};
                _config.path = ".";
                ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey",
                    "Choose Theatre File",
                    ".nt,.theatre,.nostalgiatheatre",
                    _config);
            }
            if(MenuItem("Save Theatre to File", "CTRL+S"))
                { Theatre::Current()->SaveToFile(m_sTheatreFilePath, m_sCurrentOverwriteAction); }
            BeginDisabled(not Settings::Engine::IsEditorHint);
                if(MenuItem("Run Current Theatre", "F5"))
                {
                    if(not m_sTheatreRunning or not Settings::Engine::IsEditorHint)
                        { return; }
                    mEditorTheatreData = m_spEditorTheatre->CurrentState();
                    LoadTheatre(false);
                }
            EndDisabled();
            BeginDisabled(not m_sTheatreRunning or Settings::Engine::IsEditorHint);
                if(MenuItem("Close Running Theatre", "F8"))
                    { LoadEditorTheatre(true); }
            EndDisabled();
            Separator();
            if(MenuItem("Quit", "CTRL+Q"))
                { Application()->Stop(); }
            ImGui::EndMenu();
        }
        if(BeginMenu("Edit"))
        {
            BeginDisabled(not Settings::Engine::IsEditorHint);
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
            Checkbox("Try out the new editor icons (BETA)", &sUseNewIcons);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    if(ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
    {
        if(ImGuiFileDialog::Instance()->IsOk())
        {
            m_sTheatreFilePath = ImGuiFileDialog::Instance()->GetFilePathName();
            LoadEditorTheatre(false);
        }
        ImGuiFileDialog::Instance()->Close();
    }
    if(Settings::Engine::IsEditorHint)
    {
        static ImVec2 _window_size{GetCurrentWindow()->Size};
        static bool _first{true};
        SetNextWindowSize({_window_size[0] * 0.2f, 0.0f}, (_first) ? ImGuiCond_Always : ImGuiCond_Once);
        TheatreTree();
        SameLine();
        SetNextWindowSize({_window_size[0] * 0.55f, 0.0f}, (_first) ? ImGuiCond_Always : ImGuiCond_Once);
        TheatreViewport();
        SameLine();
        TheatreInspector();
        if(_first)
            { _first = false; }
    }
    End();
}

void ImGui_Editor::LoadEditorTheatre(bool inContinue)
{
    Settings::Engine::IsEditorHint = true;
    g_pTheatreManager->SetNextTheatreType<EditorTheatre>();
    if(inContinue)
    {
        g_pTheatreManager->LoadFromData(mEditorTheatreData);
        m_sCurrentEditorTheatreSaved = false;
    }
    else
    {
        g_pTheatreManager->LoadFromFile(m_sTheatreFilePath);
        m_sCurrentEditorTheatreSaved = true;
    }
    MainWindow()->SetMouseMode(IWindow::MOUSE_MODE_VISIBLE);
}

void ImGui_Editor::LoadTheatre(bool inLoadFile)
{
    Settings::Engine::IsEditorHint = false;
    g_pTheatreManager->SetNextTheatreType<Theatre>();
    if(inLoadFile)
    {
        g_pTheatreManager->LoadFromFile(m_sTheatreFilePath);
        m_sCurrentEditorTheatreSaved = true;
    }
    else
    {
        g_pTheatreManager->LoadFromData(mEditorTheatreData);
        m_sCurrentEditorTheatreSaved = false;
    }
}

void ImGui_Editor::TheatreTree()
{
    if(not Theatre::Current()->IsStarted())
    {
        mInspectingThingUID = ID::Invalid;
        return;
    }
    BeginChild("Theatre Tree", {}, sResizableChildWithBorder);
        BeginDisabled(m_sThingAdderOpened);
            if(Button("Add Thing"))
                { m_sAddThing = true; }
        EndDisabled();
        if(CollapsingHeader("Resources"))
        {
            auto& g{*ImGui::GetCurrentContext()};
            float _indent{g.CurrentWindow->DC.CursorPos[0] + g.FontSize};
            Indent(_indent);
            for(FAUTO uid : Theatre::Current()->ResourceUIDs())
            {
                if(_select_thing(uid))
                {
                    mInspectingThingUID   = uid;
                    m_sInspectingNewThing = true;
                }
            }
            Unindent(_indent);
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
    EndChild();
}

void ImGui_Editor::TheatreInspector()
{
    static std::string _name{};
    if(not Settings::Engine::IsEditorHint)
        { return; }
    auto _variables{ThingFactory::GetThing(mInspectingThingUID)->GetVariables()};
    TheatreFile::ThingData _data{_variables->type};
    if(m_sInspectingNewThing)
    {
        m_sInspectingNewThing = false;
        _name = _variables->name;
    }

    BeginChild("Theatre Inspector", {}, ImGuiChildFlags_Borders);
        if(mInspectingThingUID.invalid())
        {
            EndChild();
            return;
        }
        if(Button("Destroy Thing"))
        {
            Theatre::Current()->DestroyThing(mInspectingThingUID);
            mInspectingThingUID = {};
            _name.clear();
            EndChild();
            return;
        }
        std::string _popup_name{std::format("Change Name")};
        if(Button(std::format("Name: {}", _name).data()))
            { OpenPopup(_popup_name.data()); }
        if(BeginPopup(_popup_name.data()))
        {
            bool _invalid{Theatre::Current()->ThingExists(_name)};
            InputText("Name", &_name);
            BeginDisabled(_invalid);
                if(Button("Change"))
                {
                    CloseCurrentPopup();
                    ThingFactory::GetThing(mInspectingThingUID)->rename(_name);
                    _variables = ThingFactory::GetThing(mInspectingThingUID)->GetVariables();
                }
            EndDisabled();
            EndPopup();
        }
        if(InspectThing(mInspectingThingUID, _variables, _data))
        {
            m_sCurrentEditorTheatreSaved = false;
            ThingFactory::GetThing(mInspectingThingUID)->SetVariables(_data);
        }
    EndChild();
}

void ImGui_Editor::TheatreViewport()
{
    static ImVec2 _viewport_window_size{}, _viewport_size{};
    bool _is_3d{true};
    Shared<Viewport> _viewport{nullptr};
    if(Manager::GetTheatreState() != ManagerEnums::IN_LEVEL or not m_spEditorTheatre)
        { return; }
    BeginChild("Editor Viewport",
        {768, 0},
        sResizableChildWithBorder);
        if(BeginTabBar("Viewports", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton))
        {
            if(BeginTabItem("3D"))
            {
                _is_3d = true;
                _viewport = m_spEditorTheatre->m_pEditor3DViewport;
                if(InputManager::IsKeyJustDown(Key::F2))
                    { m_spEditorTheatre->m_pEditor3DViewport->GetImage()->SaveJPG(m_sScreenshotFilePath); }
                EndTabItem();
            }
            if(BeginTabItem("2D"))
            {
                _is_3d = false;
                _viewport = m_spEditorTheatre->m_pEditor2DViewport;
                if(InputManager::IsKeyJustDown(Key::F2))
                    { m_spEditorTheatre->m_pEditor2DViewport->GetImage()->SaveJPG(m_sScreenshotFilePath); }
                EndTabItem();
            }
            EndTabBar();
        }
        if(not _viewport)
            { return; }
        _viewport_size = ImVec2(_viewport->Size().w(), _viewport->Size().h());
        _viewport_window_size = GetContentRegionAvail();
        GetWindowDrawList()->AddCallback(ImDrawCallback_ImplGL_EnableSRGB, nullptr);
        ImGui::Image((ImTextureRef)_viewport->GetTextureBufferID(),
            _viewport_window_size,
            {0, 1},
            {1, 0});
        GetWindowDrawList()->AddCallback(ImDrawCallback_ImplGL_DisableSRGB, nullptr);
        if(_viewport_size != _viewport_window_size)
            { _viewport->SetSize({_viewport_window_size[0], _viewport_window_size[1]}); }
        if(_is_3d)
            { Viewport3DWindow(); }
        else
            { Viewport2DWindow(); }
    EndChild();
}

bool ImGui_Editor::_select_thing(ID inUID)
{
    auto _thing{ThingFactory::GetThing(inUID)};
    ImGui::Image((ImTextureRef)GetIconTextureBufferID(ThingFactory::TypeOf(inUID)),
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
        SetItemTooltip("UID: %u", _thing->uid()());
    PopID();
    if(auto _thinker{DCast<Thinker>(_thing)})
        { _thinker->IsHighlighted(IsItemHovered()); }
    return false;
}

void ImGui_Editor::_thinker_tree_branch(ID inUID)
{
    auto children{Theatre::Current()->GetChildren(inUID)};
    if(s_TreeNodeEx(ThingFactory::GetThing(inUID)->c_name(),
        (children.empty()) ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_None))
    {
        SameLine();
        if(_select_thing(inUID))
        {
            mInspectingThingUID   = inUID;
            m_sInspectingNewThing = true;
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
            mInspectingThingUID   = inUID;
            m_sInspectingNewThing = true;
        }
    }
}

void ImGui_Editor::Viewport3DWindow()
{
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

void ImGui_Editor::SaveTheatrePopup()
{
    if(BeginPopup("Save Theatre"))
    {
        m_sSavingTheatre = true;
        InputText("Save File", &m_sTheatreFilePath);
        if(Button("Cancel"))
        {
            m_sSavingTheatre  = false;
            m_sWantNewTheatre = false;
            m_sCurrentEditorTheatreSaved = false;
            CloseCurrentPopup();
        }
        BeginDisabled(m_sTheatreFilePath.empty() or FileSystem::Exists(m_sTheatreFilePath));
            if(Button("Save"))
            {
                m_sSavingTheatre = false;
                print_error_enum(Theatre::Current()->SaveToFile(m_sTheatreFilePath));
            }
        EndDisabled();
        EndPopup();
    }
}

void ImGui_Editor::AreYouSurePopup()
{
    if(BeginPopup("Are You Sure?"))
    {
        m_sSavingTheatre= true;
        Text("Save changes made to the current Theatre?");
        if(Button("Yes"))
        {
            m_sOpenSavePopup = true;
            CloseCurrentPopup();
        }
        else if(Button("No"))
        {
            m_sSavingTheatre  = false;
            m_sWantNewTheatre = true;
            CloseCurrentPopup();
        }
        else if(Button("Cancel"))
        {
            m_sSavingTheatre  = false;
            m_sWantNewTheatre = false;
            CloseCurrentPopup();
        }
        EndPopup();
    }
}

void ImGui_Editor::CloseEditorTheatre()
{
    if(Manager::GetTheatreState() == ManagerEnums::IN_LEVEL and not m_sCurrentEditorTheatreSaved)
    {
        m_sOpenSaveBeforeExitingPopup = true;
        return;
    }
    Settings::Engine::IsEditorHint = false;
    m_spEditorTheatre = nullptr;
    m_sCurrentEditorTheatreSaved = false;
    mEditorTheatreData = TheatreFile::TheatreData{0, "UntitledTheatre"};
}

uint ImGui_Editor::GetIconTextureBufferID(FPID inType)
{
    auto& _icons{(sUseNewIcons) ? mNewEditorIcons : mEditorIcons};
    if(auto found_it{_icons.find(inType)}; found_it != _icons.end())
        { return found_it->second->Buffer()->GetID(); }
    else if(auto _base{ThingFactory::BaseOf(inType)}; not _base.invalid())
        { return GetIconTextureBufferID(_base); }
    return GetIconTextureBufferID(ThingType::Thing);
}

void ImGui_Editor::SelectThing(const char* inLabel, ID& ioUID, bool& outChanged)
{
    static const int _max_per_row{3};
    static std::string _name_input{};
    auto _label{std::format("Thing Selection##{}##{}", ioUID(), inLabel)};
    if(Button(inLabel))
        { OpenPopup(_label.data()); }
    if(BeginPopup(_label.data()))
    {
        if(Button("Clear"))
        {
            ioUID = ID::Invalid;
            outChanged = true;
            CloseCurrentPopup();
            EndPopup();
            return;
        }
        InputText("Search", &_name_input);

        int _back_of_theatre{0};
        auto _theatre_uids{Theatre::Current()->SortedThingUIDs(_back_of_theatre)};
        static auto _embedded_uids{UID::GetEmbeddedUIDs()};
        _theatre_uids.insert(_theatre_uids.begin() + _back_of_theatre,
            _embedded_uids.begin(),
            _embedded_uids.end());
        int _counter{0};
        for(size_t i{0}; i < _theatre_uids.size(); ++i)
        {
            ID uid{_theatre_uids.at(i)};
            if(uid == _theatre_uids.front())
                { SeparatorText("Thinkers"); }
            else if(i == _back_of_theatre)
                { SeparatorText("Resources"); }
            std::string _name{ThingFactory::GetName(uid)};
            if(_name.empty() or
                (not _name_input.empty() and not GetLowercase(_name).contains(GetLowercase(_name_input))))
                { continue; }
            if(Button(_name.data(), {(700.0f / _max_per_row) - 5.0f, 0.0f}))
            {
                CloseCurrentPopup();
                EndPopup();
                ioUID = uid;
                outChanged = true;
                _name_input.clear();
                ThingFactory::GetThinker(uid)->IsHighlighted(false);
                return;
            }
            ThingFactory::GetThinker(uid)->IsHighlighted(IsItemHovered());
            if(++_counter < _max_per_row and i+1 != _back_of_theatre) { SameLine(); }
            else { _counter = 0; }
        }
        EndPopup();
    }
}
