#include "imgui_editor.hpp"
#include "assets/icon_uids.hpp"
#include "things/player.hpp"
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

using namespace Icons;
using namespace ImGui;

static ImGui_Editor sImGuiEditor{};
ImGui_Editor* g_pImGuiEditor{&sImGuiEditor};

bool gTheatreInspectorActive{false};
bool gShowDebugWindow{false};
bool gDebugConsoleOpened{false};

static bool s_TreeNodeEx(const char*, ImGuiTreeNodeFlags);

static ID sSpawnLocationMaterialID{};
static ID sSpawnLocationMeshInstanceID{};
static ID sSpawnLocationID{};
static ID sEditorViewportID{};
static float sSpawnLocationRotationSpeed{1.0f};
static float sSpawnLocationScaleSpeedStore{0.0085f};
static float sSpawnLocationScaleSpeed{sSpawnLocationScaleSpeedStore};
static float sSpawnLocationScaleMax{1.0f};
static float sSpawnLocationScaleMin{0.6f};
static int sSelectedType{0};
static std::vector<const char*> sTypeNames{
    ThingType::Thing.c_name(),
    ThingType::Resource.c_name(),
    ThingType::Font.c_name(),
    ThingType::Mesh.c_name(),
    ThingType::Texture.c_name(),
    ThingType::Cubemap.c_name(),
    ThingType::ViewportTexture.c_name(),
    ThingType::Material.c_name(),
    ThingType::Thinker.c_name(),
    ThingType::Viewport.c_name(),
    ThingType::Actor3D.c_name(),
    ThingType::NostalgiaPlayer3D.c_name(),
    ThingType::Camera3D.c_name(),
    ThingType::Collider3D.c_name(),
    ThingType::Visual3D.c_name(),
    ThingType::MeshInstance3D.c_name(),
    ThingType::Sprite3D.c_name(),
    ThingType::Light3D.c_name(),
    ThingType::PointLight3D.c_name(),
    ThingType::SpotLight3D.c_name(),
    ThingType::DirectionalLight3D.c_name(),
    ThingType::Actor2D.c_name(),
    ThingType::Camera2D.c_name(),
    ThingType::Visual2D.c_name(),
    ThingType::Sprite2D.c_name(),
    ThingType::Text2D.c_name(),
};
static std::vector<PID> sTypes{
    ThingType::Thing,
    ThingType::Resource,
    ThingType::Font,
    ThingType::Mesh,
    ThingType::Texture,
    ThingType::Cubemap,
    ThingType::ViewportTexture,
    ThingType::Material,
    ThingType::Thinker,
    ThingType::Viewport,
    ThingType::Actor3D,
    ThingType::NostalgiaPlayer3D,
    ThingType::Camera3D,
    ThingType::Collider3D,
    ThingType::Visual3D,
    ThingType::MeshInstance3D,
    ThingType::Sprite3D,
    ThingType::Light3D,
    ThingType::PointLight3D,
    ThingType::SpotLight3D,
    ThingType::DirectionalLight3D,
    ThingType::Actor2D,
    ThingType::Camera2D,
    ThingType::Visual2D,
    ThingType::Sprite2D,
    ThingType::Text2D,
};

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
    if(!Settings::Engine::IsEditorHint)
    {
        auto player{Theatre::Current()->GetPlayer<EditorPlayer3D>()};
        UI_Implementor::SetGlobalCanHandleEvents(false);
        MainWindow()->SetMouseMode(IWindow::MOUSE_MODE_DISABLED);
        player->mCaptureKeyboard = true;
        player->mCaptureMouse    = true;
        return;
    }

    TheatreFile::ThingData mat_dat{ThingType::Material, "ThingAdderSpawnLocation_Material"};
    mat_dat.set_variable(glm::vec3{1.0f, 0.0f, 0.0f}, "Color");
    mat_dat.set_variable(true, "FullBright");
    sSpawnLocationMaterialID = Theatre::Current()->CreateThing(mat_dat);

    TheatreFile::ThingData mesh_inst_dat{ThingType::MeshInstance3D,"ThingAdderSpawnLocation_MeshInstance"};
    mesh_inst_dat.set_variable(UID::m_Ramiel, "Mesh");
    mesh_inst_dat.set_variable(sSpawnLocationMaterialID, "MaterialOverride");
    mesh_inst_dat.set_variable(true, "Wireframe");
    sSpawnLocationMeshInstanceID = Theatre::Current()->CreateThing(mesh_inst_dat);

    TheatreFile::ThingData spawn_dat{ThingType::Actor3D, "ThingAdderSpawnLocation"};
    spawn_dat.set_variable(sSpawnLocationMeshInstanceID, "Child");
    spawn_dat.set_variable(glm::vec3{1.0f}, "Scale");

    sSpawnLocationID = Theatre::Current()->CreateThing(spawn_dat);

    if(not Theatre::Current()->ThingExists("EditorViewport"))
        { sEditorViewportID = Theatre::Current()->CreateThing({ThingType::Viewport, "EditorViewport"}); }
    else
        { sEditorViewportID = Theatre::Current()->GetThing("EditorViewport")->uid(); }

    if(not Theatre::Current()->ThingExists("EditorCamera"))
    {
        TheatreFile::ThingData cam_dat{ThingType::Camera3D, "EditorCamera"};
        cam_dat.set_variable(glm::vec3{1.0, 5.0, 1.0}, "Position");
        cam_dat.set_variable(glm::vec3{-40.0, 10.0, 0.0}, "RotationDegrees");
        cam_dat.set_variable(true, "UseDefaultSkybox");
        cam_dat.set_variable(true, "Current");
        Theatre::Current()->CreateThing(cam_dat);
    }
    Theatre::Current()->SetParent(Theatre::Current()->GetThing("EditorCamera")->uid(), sEditorViewportID);
}

void ImGui_Editor::TheatreExited()
{
    sSpawnLocationMaterialID = ID{};
    sSpawnLocationMeshInstanceID = ID{};
    sSpawnLocationID = ID{};
    sEditorViewportID = ID{};
    auto player{g_pTheatreManager->Current()->GetPlayer<EditorPlayer3D>()};
    UI_Implementor::SetGlobalCanHandleEvents(true);
    MainWindow()->SetMouseMode(IWindow::MOUSE_MODE_VISIBLE);
    player->mCaptureMouse    = false;
    player->mCaptureKeyboard = false;
}

void ImGui_Editor::Input(InputEvent* event)
{
    if(MainWindow()->GetMouseMode() == IWindow::MOUSE_MODE_DISABLED)
    {
        if(event->IsJustPressed(Key::Escape))
        {
            auto player{g_pTheatreManager->Current()->GetPlayer<EditorPlayer3D>()};
            UI_Implementor::SetGlobalCanHandleEvents(true);
            MainWindow()->SetMouseMode(IWindow::MOUSE_MODE_VISIBLE);
            player->mCaptureMouse    = false;
            player->mCaptureKeyboard = false;
        }
        return;
    }
    else if(event->IsJustPressed(Key::D) and event->IsModifierActive(Key::Mod_Control | Key::Mod_Shift)
        and Manager::GetTheatreState() == ManagerEnums::IN_LEVEL)
    {
        auto player{g_pTheatreManager->Current()->GetPlayer<EditorPlayer3D>()};
        UI_Implementor::SetGlobalCanHandleEvents(false);
        MainWindow()->SetMouseMode(IWindow::MOUSE_MODE_DISABLED);
        player->mCaptureKeyboard = true;
        player->mCaptureMouse    = true;
    }
    else if(event->IsJustPressed(Key::D) and event->IsModifierActive(Key::Mod_Control))
        { gShowDebugWindow = !gShowDebugWindow; }
}

enum ScaleDir
{
    SCALE_DIRECTION_UP   = 1,
    SCALE_DIRECTION_DOWN = -1
};
static ScaleDir sScaleDirection{SCALE_DIRECTION_DOWN};

void ImGui_Editor::Update()
{
    if(not Settings::Engine::IsEditorHint or
            not Theatre::Current() or
            not Theatre::Current()->IsStarted())
        { return; }
    static ImGuiChildFlags sResizableChildWithBorder{ImGuiChildFlags_Borders |
        ImGuiChildFlags_ResizeY |
        ImGuiChildFlags_ResizeX};
    if(!sSpawnLocationID.invalid())
    {
        auto thingy{g_pTheatreManager->Current()->GetThinker<Actor3D>(sSpawnLocationMeshInstanceID)};
        if(thingy->Scale().y > sSpawnLocationScaleMax)
            { sScaleDirection = SCALE_DIRECTION_DOWN; }
        else if(thingy->Scale().y < sSpawnLocationScaleMin)
            { sScaleDirection = SCALE_DIRECTION_UP; }
        sSpawnLocationScaleSpeed = (int)sScaleDirection * sSpawnLocationScaleSpeedStore;
        thingy->SetRotationDegrees(thingy->RotationDegrees() + glm::vec3{0.0f, sSpawnLocationRotationSpeed, 0.0f});
        thingy->SetScale(thingy->Scale() + glm::vec3{0.0f, sSpawnLocationScaleSpeed, 0.0f});
    }
    if(Begin("Nostalgia Editor", nullptr, ImGuiWindowFlags_MenuBar))
    {
        if(BeginMenuBar())
        {
            if(BeginMenu("Menu"))
            {
                if(MenuItem("Debug Menu", "CTRL+D"))
                    { gShowDebugWindow = true; }
                BeginDisabled(Manager::GetTheatreState() != ManagerEnums::IN_LEVEL);
                if(MenuItem("Theatre Inspector", "F3"))
                    { gTheatreInspectorActive = true; }
                EndDisabled();
                if(MenuItem("Debug Console", "~"))
                    { gDebugConsoleOpened = true; }
                if(MenuItem("Quit", "CTRL+Q"))
                    { Application()->Stop(); }
                ImGui::EndMenu();
            }            EndMenuBar();
        }
        if(Manager::GetTheatreState() != ManagerEnums::IN_LEVEL)
            { End(); return; }
        auto viewport{g_pTheatreManager->Current()->GetThinker<Viewport>(sEditorViewportID)};
        if(viewport->uid().invalid())
            { End(); return; }
        BeginChild("LeftSide",
            {0, 0},
            sResizableChildWithBorder);
            BeginChild("Viewport",
                {500, 500},
                sResizableChildWithBorder,
                ImGuiWindowFlags_NoScrollbar |
                    ImGuiWindowFlags_NoScrollWithMouse);
                GetWindowDrawList()->AddCallback(ImDrawCallback_ImplGL_EnableSRGB, nullptr);
                ImGui::Image((ImTextureID)viewport->Framebuffer()->TextureID(),
                    {(float)viewport->Size().w(), (float)viewport->Size().h()},
                    {0, 1},
                    {1, 0});
                GetWindowDrawList()->AddCallback(ImDrawCallback_ImplGL_DisableSRGB, nullptr);
            EndChild();
            static bool camera_moving{false};
            auto player{g_pTheatreManager->Current()->GetPlayer<EditorPlayer3D>()};
            if((IsItemHovered() or camera_moving)
                and !player->mCaptureMouse)
            {
                if(InputManager::IsKeyDown(Key::MouseLeft) and !camera_moving)
                    { UI_Implementor::SetGlobalCanHandleEvents(false); camera_moving = true; MainWindow()->SetMouseMode(IWindow::MouseMode::MOUSE_MODE_DISABLED); }
                if(InputManager::IsKeyDown(Key::MouseLeft))
                {
                    auto camera{g_pTheatreManager->Current()->GetThinker<Camera3D>(viewport->CurrentCamera3D())};
                    auto motion{InputManager::MouseMotion()};
                    camera->SetRotationDegrees(camera->RotationDegrees() - (glm::vec3{motion.y(), motion.x(), 0.0f} * 0.1f));
                    glm::vec2 movement_direction{InputManager::IsActionDown("+right") - InputManager::IsActionDown("+left"),
                        InputManager::IsActionDown("+forward") - InputManager::IsActionDown("+backward")};

                    camera->SetPosition(camera->Position() +
                        ((
                            ((camera->Quaternion() * Settings::World::Front()) *
                                movement_direction[1]) +
                            ((camera->Quaternion() * Settings::World::Right()) *
                                movement_direction[0])
                        ) * 0.1f)
                    );
                }
            }
            if(InputManager::IsKeyUp(Key::MouseLeft)
                and camera_moving
                and !player->mCaptureMouse)
                { UI_Implementor::SetGlobalCanHandleEvents(true); camera_moving = false; MainWindow()->SetMouseMode(IWindow::MouseMode::MOUSE_MODE_VISIBLE); }
            BeginChild("AddThingMenu", {500, 690}, sResizableChildWithBorder);
                ThingAdder();
            EndChild();
        EndChild();
        SameLine();
        TheatreInspector();
    }
    End();
}

void ImGui_Editor::ThingAdder()
{
    if(!Settings::Engine::IsEditorHint)
        { return; }
    auto theatre{g_pTheatreManager->Current()};
    auto spawn_locator{theatre->GetThinker<Actor3D>(sSpawnLocationID)};
    BeginChild("MakeNewThing");
    if(CollapsingHeader("Thing Spawner Widget"))
    {
        DragFloat("Spawn Location Rotation Speed", &sSpawnLocationRotationSpeed, 0.001f, 0.0f, 0.0f, "%.2f");
        DragFloat("Spawn Location Scale Speed", &sSpawnLocationScaleSpeedStore, 0.1f, 0.0f, 0.0f, "%.2f");
        DragFloat("Spawn Location Scale Max", &sSpawnLocationScaleMax, 0.1f, 0.0f, 0.0f, "%.2f");
        DragFloat("Spawn Location Scale Min", &sSpawnLocationScaleMin, 0.1f, 0.0f, 0.0f, "%.2f");
    }
    SeparatorText("Thing Spawner");
    static TheatreFile::ThingData thing_dat{};
    static std::string thing_name{};
    InputTextWithHint("Name", "UntitledThing", &thing_name);
    if(BeginCombo("Type", sTypeNames[sSelectedType], ImGuiComboFlags_WidthFitPreview))
    {
        for(uint i{0}; i < sTypeNames.size(); ++i)
        {
            const bool is_selected{sSelectedType == i};
            if(Selectable(sTypeNames[i], is_selected))
                { sSelectedType = i; }
            if(is_selected)
                { SetItemDefaultFocus(); }
        }
        EndCombo();
    }
    static glm::vec3 spawn_location{0.0f},
        spawn_rotation{0.0f},
        spawn_scale{0.0f};
    if(ThingFactory::IsDerivedFrom(sTypes[sSelectedType], ThingType::Actor3D))
    {
        if(DragGLMv3("Position", &spawn_location, 0.01f, 0.0f, 0.0f, "%.2f"))
            { spawn_locator->SetPosition(spawn_location); }
        if(DragGLMv3("Rotation (Degrees)", &spawn_rotation, 0.01f, 0.0f, 0.0f, "%.2f"))
            { spawn_locator->SetRotationDegrees(spawn_rotation); }
        if(DragGLMv3("Scale", &spawn_scale, 0.01f, 0.0f, 0.0f, "%.2f"))
            { spawn_locator->SetScale(spawn_scale); }
        if(Button("Reset Spawn Transform"))
        {
            spawn_locator->SetPosition(spawn_location = glm::vec3{0.0f});
            spawn_locator->SetRotationDegrees(spawn_rotation = glm::vec3{0.0f});
            spawn_locator->SetScale(spawn_scale = glm::vec3{1.0f});
        }
    }
    if(Button("Spawn Thing"))
    {
        thing_dat = {sTypes[sSelectedType], (thing_name.empty()) ? "UntitledThing" : thing_name};
        thing_dat.set_variable(spawn_location, "Position");
        thing_dat.set_variable(spawn_rotation, "RotationDegrees");
        thing_dat.set_variable(spawn_scale, "Scale");
        theatre->CreateThing(thing_dat);
        thing_name.clear();
        thing_dat.clear();
    }

    EndChild();
}

void ImGui_Editor::TheatreInspector()
{
    if(not Theatre::Current()->IsStarted())
    {
        mInspectingResourceUID = mInspectingThinkerUID = ID::Invalid;
        return;
    }
    BeginChild("TheatreInspector");
        if(CollapsingHeader("Resources"))
        {
            for(FAUTO uid : Theatre::Current()->ResourceUIDs())
                { _resource_list_item(uid); }
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
        {
            if(mInspectingThinkerName.empty())
                { mInspectingThinkerName = Theatre::Current()->GetName(mInspectingThinkerUID); }
            InspectThinker();
        }
        if(not mInspectingResourceUID.invalid())
        {
            if(mInspectingResourceName.empty())
                { mInspectingResourceName = Theatre::Current()->GetName(mInspectingResourceUID); }
            InspectResource();
        }
    EndChild();
}

void ImGui_Editor::_display_thinker(ID inUID)
{
    auto _thinker{Theatre::Current()->GetThinker(inUID)};
    SameLine();
    ImGui::Image((ImTextureRef)GetIconTextureBufferID(Theatre::Current()->TypeOf(inUID)),
        {30, 30},
        {0, 1},
        {1, 0});
    SameLine();
    PushID(inUID());
    if(Button(_thinker->c_name(), {0, 30}))
        { mInspectingThinkerUID = inUID; }
    PopID();
    _thinker->IsHighlighted(IsItemHovered());
}

void ImGui_Editor::_thinker_tree_branch(ID inUID)
{
    auto children{Theatre::Current()->GetChildren(inUID)};
    if(s_TreeNodeEx(Theatre::Current()->GetThing(inUID)->c_name(),
        (children.empty()) ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_None))
    {
        _display_thinker(inUID);
        for(ID child : children)
            { _thinker_tree_branch(child); }
        TreePop();
    }
    else
        { _display_thinker(inUID); }
}

void ImGui_Editor::_resource_list_item(ID inUID)
{
    bool _pressed{ImageButton(std::format("Resource##{}", inUID()).data(),
        GetIconTextureBufferID(Theatre::Current()->TypeOf(inUID)),
        {30, 30},
        {0, 1},
        {1, 0})};
    SameLine();
    _pressed = Button(Theatre::Current()->GetThing(inUID)->c_name(), {0, 30});
    if(_pressed)
        { mInspectingResourceUID = inUID; }
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
    else if(_type == ThingType::NostalgiaPlayer3D)
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
