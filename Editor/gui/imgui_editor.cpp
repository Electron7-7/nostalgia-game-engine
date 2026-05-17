#include "./imgui_editor.hpp"
#include "./imgui_debugger.hpp"
#include "editor_icons.hpp"
#include "theatre/editor_theatre.hpp"
#include "thirdparty/DearImGui/imgui.h"
#include "thirdparty/DearImGui/imgui_stdlib.h"
#include "thirdparty/DearImGui/imgui_internal.h"
#include <Nostalgia/application/application.hpp>
#include <Nostalgia/managers/theatre_manager.hpp>
#include <Nostalgia/managers/resource_manager.hpp>
#include <Nostalgia/events/event.hpp>
#include <Nostalgia/managers/input_manager.hpp>
#include <Nostalgia/rendering/renderer_api.hpp>
#include <Nostalgia/settings/engine.hpp>
#include <Nostalgia/settings/world.hpp>
#include <Nostalgia/ui/implementor.hpp>
#include <Nostalgia/things/thing_factory.hpp>
#include <Nostalgia/things/thinkers/viewport.hpp>
#include <Nostalgia/things/thinkers/3d/ramiel.hpp>

#define REGISTER_ICON(TYPE, VAR_NAME, THING_NAME) \
    m_sEditorIcons[TYPE] = ImageTexture::CreateFromImage(\
        Image::CreateFromFile({_EditorIcons::VAR_NAME,std::size(_EditorIcons::VAR_NAME)})); \
    m_sEditorIcons[TYPE]->rename(#THING_NAME);

using namespace ImGui;

static ImGui_Editor sImGuiEditor{};
ImGui_Editor* g_pImGuiEditor{&sImGuiEditor};

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
    REGISTER_ICON(ThingType::Actor2D, actor_2d, Actor2D)
    REGISTER_ICON(ThingType::Actor3D, actor_3d, Actor3D)
    REGISTER_ICON(ThingType::ArrayMesh, array_mesh, ArrayMesh)
    REGISTER_ICON(ThingType::Camera2D, camera_2d, Camera2D)
    REGISTER_ICON(ThingType::Camera3D, camera_3d, Camera3D)
    REGISTER_ICON(ThingType::Collider3D, collider_3d, Collider3D)
    REGISTER_ICON(ThingType::Cubemap, cubemap, Cubemap)
    REGISTER_ICON(ThingType::DirectionalLight3D, directional_light_3d, DirectionalLight3D)
    REGISTER_ICON(ThingType::Font, font, Font)
    REGISTER_ICON(ThingType::Image, image, Image)
    REGISTER_ICON(ThingType::ImageTexture, image_texture, ImageTexture)
    REGISTER_ICON(ThingType::Light3D, light_3d, Light3D)
    REGISTER_ICON(ThingType::Material, material, Material)
    REGISTER_ICON(ThingType::Mesh, mesh, Mesh)
    REGISTER_ICON(ThingType::MeshInstance3D, mesh_instance_3d, MeshInstance3D)
    REGISTER_ICON(ThingType::NostalgiaPlayer, nostalgia_player, NostalgiaPlayer)
    REGISTER_ICON(ThingType::PointLight3D, point_light_3d, PointLight3D)
    REGISTER_ICON(ThingType::Resource, resource, Resource)
    REGISTER_ICON(ThingType::SpotLight3D, spot_light_3d, SpotLight3D)
    REGISTER_ICON(ThingType::Sprite2D, sprite_2d, Sprite2D)
    REGISTER_ICON(ThingType::Sprite3D, sprite_3d, Sprite3D)
    REGISTER_ICON(ThingType::Text2D, text_2d, Text2D)
    REGISTER_ICON(ThingType::Texture, texture, Texture)
    REGISTER_ICON(ThingType::Thing, thing, Thing)
    REGISTER_ICON(ThingType::Thinker, thinker, Thinker)
    REGISTER_ICON(ThingType::Viewport, viewport, Viewport)
    REGISTER_ICON(ThingType::ViewportTexture, viewport_texture, ViewportTexture)
    REGISTER_ICON(ThingType::Visual2D, visual_2d, Visual2D)
    REGISTER_ICON(ThingType::Visual3D, visual_3d, Visual3D)
    REGISTER_ICON(Ramiel::sClassType(), ramiel, Ramiel)

    mAskAreYouSure = false;
    mIsEditorSaved = true;

    g_pTheatreManager->SetNextTheatreType<EditorTheatre>();
    if(mCurrentTheatreFilePath.empty())
        { g_pTheatreManager->LoadFromData(mEditorTheatreData = {}); }
    else
        { g_pTheatreManager->LoadFromFile(mCurrentTheatreFilePath); }
}

void ImGui_Editor::Shutdown()
{
    PRINT_PRETTY_FUNCTION;
    m_sEditorIcons.clear();
}

void ImGui_Editor::TheatreEntered()
{
    if(Settings::Engine::IsEditorHint)
    {
        Console::SetVariable("mat_fullbright", mViewTheatreInFullbright);
        mAskAreYouSure = not mIsEditorSaved;
        mEditorTheatreData = Theatre::Current()->CurrentState();
    }
}

void ImGui_Editor::TheatreExited()
{ mInspectingThingUID = {}; }

void ImGui_Editor::SetStartupTheatre(Sarg inFilePath)
{ mCurrentTheatreFilePath = inFilePath; }

bool ImGui_Editor::ReadyToQuit()
{ return mReadyToQuit; }

void ImGui_Editor::Input(InputEvent* event)
{
    if(event->IsJustPressed(Key::Q) and event->IsModifierActive(Key::Mod_Control))
        { QuitEditor(); }
    else if(event->IsJustPressed(Key::D) and event->IsModifierActive(Key::Mod_Control))
        { ImGui_Debugger::m_sDebugWindowOpened = !ImGui_Debugger::m_sDebugWindowOpened; }
    else if(event->IsJustPressed(Key::N) and event->IsModifierActive(Key::Mod_Control))
        { CreateNewTheatre(); }
    else if(event->IsJustPressed(Key::F2) and mTheatreRunning and not Settings::Engine::IsEditorHint)
        { Theatre::Current()->TakeScreenshot()->SaveJPG(mScreenshotFilePath); }
    else if(event->IsJustPressed(Key::F5))
    {
        mAskAreYouSure = false;
        PlayCurrentTheatre();
    }
    else if(event->IsJustPressed(Key::F8))
    {
        mAskAreYouSure = false;
        ExitBackToEditor();
    }
    else if(event->IsJustPressed(Key::O) and event->IsModifierActive(Key::Mod_Control))
        { LoadNewTheatre(); }
    else if(event->IsJustPressed(Key::S) and event->IsModifierActive(Key::Mod_Control))
        { SaveCurrentTheatre(); }
    else if(Settings::Engine::IsEditorHint
        and event->IsJustPressed(Key::A)
        and event->IsModifierActive(Key::Mod_Control)
        and event->IsModifierActive(Key::Mod_Shift))
        { mAddThing = true; }
}

void ImGui_Editor::Event(IEvent* inEvent)
{
    if(inEvent->IsEvent(WindowEvent::WindowResize))
        { mResizeEditorWindows = true; }
}

void ImGui_Editor::Update()
{
    mTheatreRunning = Manager::GetTheatreState() == ManagerEnums::IN_LEVEL;
    do_TheatreRelatedPopups();
    if(not Settings::Engine::IsEditorHint
        and Application()->MainWindow()->GetMouseMode() != IWindow::MOUSE_MODE_VISIBLE)
        { return; }
    SetNextWindowPos({0,0}, ImGuiCond_Once);
    SetNextWindowSize({static_cast<float>(Application()->MainWindow()->GetWidth()),
        static_cast<float>(Application()->MainWindow()->GetHeight())});
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
    do_ThingAdder();
    if(BeginMenuBar())
    {
        if(BeginMenu("File"))
        {
            if(MenuItem("New Theatre", "CTRL+N"))
                { CreateNewTheatre(); }
            if(MenuItem("Open Theatre File", "CTRL+O"))
                { LoadNewTheatre(); }
            if(MenuItem("Save Theatre to File", "CTRL+S"))
                { SaveCurrentTheatre(); }
            BeginDisabled(not Settings::Engine::IsEditorHint);
                if(MenuItem("Run Current Theatre", "F5"))
                    { PlayCurrentTheatre(); }
            EndDisabled();
            BeginDisabled(Settings::Engine::IsEditorHint);
                if(MenuItem("Close Running Theatre", "F8"))
                    { ExitBackToEditor(); }
            EndDisabled();
            Separator();
            if(MenuItem("Quit", "CTRL+Q"))
                { QuitEditor(); }
            ImGui::EndMenu();
        }
        if(BeginMenu("Edit"))
        {
            BeginDisabled(not Settings::Engine::IsEditorHint);
                if(MenuItem("Add Thing", "CTRL+SHIFT+A"))
                    { mAddThing = true; }
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
            if(Checkbox("Render Editor Viewports in Fullbright", &mViewTheatreInFullbright))
                { Console::SetVariable("mat_fullbright", mViewTheatreInFullbright); }
            SliderFloat("2D Camera Movement Speed", &s2DCameraMovementSpeed, 1.0f, 100.0f);
            SliderFloat("2D Camera Zooming Factor", &s2DCameraZoomFactor, 0.001f, 0.999f);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    if(Settings::Engine::IsEditorHint and mTheatreRunning)
    {
        static ImVec2 _window_size{};
        static float _theatre_tree_ratio{0.23f}, _theatre_viewport_ratio{0.55f};
        if(mResizeEditorWindows)
            { _window_size = GetCurrentWindow()->Size; }
        SetNextWindowSize({_window_size[0] * _theatre_tree_ratio, 0.0f},
            (mResizeEditorWindows) ? ImGuiCond_Always : ImGuiCond_Once);
        TheatreTree();
        SameLine();
        SetNextWindowSize({_window_size[0] * _theatre_viewport_ratio, 0.0f},
            (mResizeEditorWindows) ? ImGuiCond_Always : ImGuiCond_Once);
        TheatreViewport();
        SameLine();
        TheatreInspector();
        if(mResizeEditorWindows)
            { mResizeEditorWindows = false; }
    }
    End();
}

void ImGui_Editor::TheatreTree()
{
    if(not Theatre::Current()->IsStarted())
    {
        mInspectingThingUID = ID::Invalid;
        return;
    }
    BeginChild("Theatre Tree", {}, sResizableChildWithBorder);
        BeginDisabled(mThingAdderOpened);
            mAddThing = Button("Add Thing");
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
                    mInspectingThingUID = uid;
                    mInspectingNewThing = true;
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
    static std::string _name_override{};
    std::string _name{ThingFactory::GetName(mInspectingThingUID)};
    if(not Settings::Engine::IsEditorHint)
        { return; }
    else if(mInspectingNewThing)
        { _name_override = _name; }

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
            bool _invalid{Theatre::Current()->Contains(_name_override)};
            InputText("Name", &_name_override);
            BeginDisabled(_invalid);
                if(Button("Change"))
                {
                    CloseCurrentPopup();
                    ThingFactory::GetThing(mInspectingThingUID)->rename(_name_override);
                }
            EndDisabled();
            EndPopup();
        }
        else
            { _name_override = _name; }
        InspectThing();
    EndChild();
}

void ImGui_Editor::TheatreViewport()
{
    static ImVec2 _viewport_window_size{}, _viewport_size{};
    EditorTheatre* _theatre{nullptr};
    bool _is_3d{true};
    Shared<Viewport> _viewport{nullptr};
    if(not Settings::Engine::IsEditorHint
        or Manager::GetTheatreState() != ManagerEnums::IN_LEVEL
        or not (_theatre = dynamic_cast<EditorTheatre*>(Theatre::Current())))
        { return; }
    BeginChild("Editor Viewport",
        {768, 0},
        sResizableChildWithBorder);
    if(BeginTabBar("Viewports", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton))
    {
        if(BeginTabItem("3D"))
        {
            _is_3d = true;
            _viewport = _theatre->m_pEditor3DViewport;
            if(InputManager::IsKeyJustDown(Key::F2))
                { _theatre->m_pEditor3DViewport->GetImage()->SaveJPG(mScreenshotFilePath); }
            EndTabItem();
        }
        if(BeginTabItem("2D"))
        {
            _is_3d = false;
            _viewport = _theatre->m_pEditor2DViewport;
            if(InputManager::IsKeyJustDown(Key::F2))
                { _theatre->m_pEditor2DViewport->GetImage()->SaveJPG(mScreenshotFilePath); }
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
        { Viewport3DWindow(_theatre); }
    else
        { Viewport2DWindow(_theatre); }
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
            mInspectingThingUID = inUID;
            mInspectingNewThing = true;
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
            mInspectingThingUID = inUID;
            mInspectingNewThing = true;
        }
    }
}

void ImGui_Editor::Viewport3DWindow(EditorTheatre* ioTheatre)
{
    static bool camera_moving{false};
    if(not ioTheatre)
        { return; }
    else if((IsItemHovered() or camera_moving))
    {
        if(InputManager::IsKeyDown(Key::MouseLeft) and !camera_moving)
        {
            UI_Implementor::SetGlobalCanHandleEvents(false);
            camera_moving = true;
            Application()->MainWindow()->SetMouseMode(IWindow::MouseMode::MOUSE_MODE_DISABLED);
        }
        if(InputManager::IsKeyDown(Key::MouseLeft))
        {
            auto motion{InputManager::MouseMotion()};
            ioTheatre->m_pEditorCamera3D
                ->SetRotationDegrees(ioTheatre->m_pEditorCamera3D->RotationDegrees() -
                    (glm::vec3{motion.y(), motion.x(), 0.0f} * 0.1f));
            glm::vec2 movement_direction{
                InputManager::IsActionDown("+right") - InputManager::IsActionDown("+left"),
                InputManager::IsActionDown("+forward") - InputManager::IsActionDown("+backward")};

            ioTheatre->m_pEditorCamera3D->SetPosition(ioTheatre->m_pEditorCamera3D->Position() +
                ((
                    ((ioTheatre->m_pEditorCamera3D->Quaternion() * Settings::World::Front()) *
                        movement_direction[1]) +
                    ((ioTheatre->m_pEditorCamera3D->Quaternion() * Settings::World::Right()) *
                        movement_direction[0])
                ) * 0.1f)
            );
        }
    }
    if(InputManager::IsKeyUp(Key::MouseLeft) and camera_moving)
    {
        UI_Implementor::SetGlobalCanHandleEvents(true);
        camera_moving = false;
        Application()->MainWindow()->SetMouseMode(IWindow::MouseMode::MOUSE_MODE_VISIBLE);
    }
}

void ImGui_Editor::Viewport2DWindow(EditorTheatre* ioTheatre)
{
    static bool camera_moving{false};
    if(not ioTheatre)
        { return; }
    else if((IsItemHovered() or camera_moving))
    {
        if(InputManager::IsKeyDown(Key::MouseLeft) and not camera_moving)
        {
            UI_Implementor::SetGlobalCanHandleEvents(false);
            camera_moving = true;
            Application()->MainWindow()->SetMouseMode(IWindow::MouseMode::MOUSE_MODE_DISABLED);
        }
        if(InputManager::IsKeyDown(Key::MouseLeft))
        {
            auto _scroll{InputManager::ScrollOffset()[1]};
            auto _zoom{ioTheatre->m_pEditorCamera2D->Zoom()};
            float _zoom_min{(_zoom[1] > _zoom[0]) ? _zoom[0] : _zoom[1]};
            if(_scroll)
            {
                float _zoom_factor{static_cast<float>(_scroll) * s2DCameraZoomFactor * abs(_zoom_min)};
                if(_scroll < 0 and _zoom_min < 1)
                    { _zoom_factor *= 0.5f; }
                ioTheatre->m_pEditorCamera2D->SetZoom(_zoom += glm::vec2{_zoom_factor});
            }
            if(auto motion{InputManager::MouseMotion()}; not motion.is_zero())
            {
                ioTheatre->m_pEditorCamera2D
                    ->SetPosition(ioTheatre->m_pEditorCamera2D->Position() -
                        glm::vec2{motion.x(), -motion.y()} * s2DCameraMovementSpeed / _zoom);
            }
        }
    }
    if(InputManager::IsKeyUp(Key::MouseLeft) and camera_moving)
    {
        UI_Implementor::SetGlobalCanHandleEvents(true);
        camera_moving = false;
        Application()->MainWindow()->SetMouseMode(IWindow::MouseMode::MOUSE_MODE_VISIBLE);
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
    if(auto found_it{m_sEditorIcons.find(inType)}; found_it != m_sEditorIcons.end())
        { return found_it->second->Buffer()->GetID(); }
    else if(auto _base{ThingFactory::BaseOf(inType)}; not _base.invalid())
        { return GetIconTextureBufferID(_base); }
    return GetIconTextureBufferID(ThingType::Thing);
}

bool ImGui_Editor::SelectThing(const char* inLabel, ID& ioUID)
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
            CloseCurrentPopup();
            EndPopup();
            return true;
        }
        InputText("Search", &_name_input);

        int _back_of_theatre{0};
        auto _theatre_uids{Theatre::Current()->SortedThingUIDs(_back_of_theatre)};
        static auto _embedded_uids{g_pResourceManager->GetEmbeddedResources()};
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
                _name_input.clear();
                ThingFactory::GetThinker(uid)->IsHighlighted(false);
                return true;
            }
            ThingFactory::GetThinker(uid)->IsHighlighted(IsItemHovered());
            if(++_counter < _max_per_row and i+1 != _back_of_theatre) { SameLine(); }
            else { _counter = 0; }
        }
        EndPopup();
    }
    return false;
}
